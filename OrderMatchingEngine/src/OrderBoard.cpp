/*
 * OrderBoard.cpp
 *
 *  Created on: 2022Äê7ÔÂ9ÈÕ
 *      Author: yufei
 */

#include <iomanip>
#include <iostream>
#include <stdexcept>


#include "OrderBoard.h"

using namespace std;
using namespace ENGINE;


const int MAX_PRICE = 999999999;
const int MAX_SEQ = 999999999;

OrderRequest::OrderRequest(RequestType_t requestType, const Order& order)
:myRequestType(requestType),myOrder(order) {

};

OrderBoard::OrderBoard():myOrderSequence(0),myStopFlag(false) {
   myQueueReader = thread(processMsg,this);
}

OrderBoard::~OrderBoard() {
   std::unique_lock<mutex> locker(myQueueMutex);
   myStopFlag=true;
   myQueueSignal.notify_all();
   locker.unlock();

   if(myQueueReader.joinable() ) {
	   myQueueReader.join();
   }
}

void OrderBoard::submitNew(const Order& order) {
	std::unique_lock<mutex> locker(myQueueMutex);
	myRequestQueue.emplace(RequestType_t::new_e, order);
	myQueueSignal.notify_all();
}

void OrderBoard::submitCancel(const Order& order) {
	std::unique_lock<mutex> locker(myQueueMutex);
	myRequestQueue.emplace(RequestType_t::cancel_e, order);
	myQueueSignal.notify_all();
}

OrderEvent OrderBoard::getResultsOfLastOperation(const Order& order) {
	const auto& itr = myClientIdMap.find(order.getClientId());
	string exchangeId = (itr == myClientIdMap.end())? "NONE" : itr->second;
	return myOrderHouse.queryOrder(exchangeId);
}

void OrderBoard::processMsg() {
	while(true) {
		std::unique_lock<mutex> locker(myQueueMutex);

		if(myRequestQueue.empty()) {
			myQueueSignal.wait(locker);
		}

		if(myStopFlag) {
			break;
		}

		OrderRequest request = myRequestQueue.front();
		myRequestQueue.pop();

		switch (request.myRequestType) {
		case RequestType_t::new_e:
			newOrder(request.myOrder);
		    break;
		case RequestType_t::cancel_e:
			cancelOrder(request.myOrder.getClientId());
			break;
		default:
			break;
		}

	}
}

void OrderBoard::newOrder(const Order& order) {
	cout <<"OrderBoard process new order " << order.getClientId() <<endl;

	//add to order house
    string exchangeId = myOrderHouse.addOrder(order);

    //keep a map between clientId and exchangeId
	myClientIdMap[order.getClientId()] = exchangeId;

    //execute it first, then place remaining order on the execution book
	string matchKey = createOrderMachingKey(order);
	cout << "Using matchKey " << matchKey << " for order " << exchangeId << " to execute on board" << endl;
    auto& books = myExecutionBooks[order.getInstrument()];

    ExecutionBook* pPlaceBook = &books.first; //buy order list
    ExecutionBook* pMatchBook = &books.second; //sell order list
    if ( order.getSide() == OrderSide_t::sell_e) {
    	pPlaceBook = &books.second;
    	pMatchBook = &books.first;
    }

    auto itr= pMatchBook->begin();
    int remainingQuantity = order.getQuantity();

    while (itr != pMatchBook->end() && itr->first <= matchKey ) {

    	//quantity to execute
    	int execQuantity  = min(itr->second.second, remainingQuantity);

    	//execute both orders in the house
    	myOrderHouse.executeOrder(exchangeId, execQuantity);
    	myOrderHouse.executeOrder(itr->second.first, execQuantity);

    	//update new order
    	remainingQuantity -= execQuantity;

    	//update the order on the book
    	itr->second.second -= execQuantity;
    	if(itr->second.second == 0) {
    		itr = pMatchBook->erase(itr);
    	}

    	if (remainingQuantity == 0) {
    		break;
    	}
    }

    //leave limit order on the board if not full executed
    if (remainingQuantity > 0 && order.getPrice() > 0 ) {
       string sortKey = createOrderSortKey(order);
       (*pPlaceBook)[sortKey] = make_pair(exchangeId, remainingQuantity);

       cout << "Put order " << exchangeId << " on board with sortKey " << sortKey <<
    		   " for instrument " << order.getInstrument() << endl;
       myClientIdSortKeyMap[order.getClientId()] = sortKey;
    }
}

void OrderBoard::cancelOrder(const string& clientId) {
	cout <<"OrderBoard process order cancel " << clientId <<endl;

	const auto& itr = myClientIdMap.find(clientId);
	string exchangeId = itr == myClientIdMap.end()? "NONE" : itr->second;
	myOrderHouse.cancelOrder(exchangeId);

	if(	myClientIdSortKeyMap.erase(clientId) > 0 ) {
		cout <<"OrderBoard erase order " << clientId <<endl;
	} else {
		cout << "Order " <<clientId << " is not on board" << endl;
	}
}

std::string OrderBoard::createOrderSortKey(const Order& order) {
	std::ostringstream ss;
	int price = order.getPrice();
	if(order.getSide() == OrderSide_t::buy_e && order.getPrice() > 0) {
		price = MAX_PRICE - price;
	}
	ss<< std::setfill('0') << std::setw(9) << price;
	ss<< std::setfill('0') << std::setw(9) << myOrderSequence++;
	return ss.str();
}

std::string OrderBoard::createOrderMachingKey(const Order& order) const {
	std::ostringstream ss;
	int price = MAX_PRICE - order.getPrice();
	if(order.getSide() == OrderSide_t::buy_e && order.getPrice() > 0) {
		price = order.getPrice();
	}
	ss<< std::setfill('0') << std::setw(9) << price;
	ss<< std::setfill('0') << std::setw(9) << MAX_SEQ;
	return ss.str();
}



