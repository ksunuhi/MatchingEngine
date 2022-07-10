/*
 * OrderHouse.cpp
 *
 *  Created on: 2022Äê7ÔÂ9ÈÕ
 *      Author: yufei
 */

#include <iostream>
#include <iomanip>
#include "OrderHouse.h"

using namespace std;
using namespace ENGINE;

OrderHouseLock::OrderHouseLock()
:myReader(0), myWriter(false) {
}

OrderHouseLock::OrderHouseLock(const OrderHouseLock&)
:myReaderSignal(),myWriterSignal(),myReader(0), myWriter(false) {

}

void OrderHouseLock::readWait() {
	std::unique_lock<mutex> locker(myMutex);
    while (myWriter) {
    	myWriterSignal.wait(locker);
    }
    myReader++;
}

void OrderHouseLock::readDone() {
	std::unique_lock<mutex> locker(myMutex);
    myReader--;
    if (myReader == 0) {
    	myReaderSignal.notify_all();
    }
}

void OrderHouseLock::writeWait() {
	std::unique_lock<mutex> locker(myMutex);
    while (myWriter>0 ) {
    	myWriterSignal.wait(locker);
    }
    while (myReader >0) {
    	myReaderSignal.wait(locker);
    }
    myWriter = true;
}

void OrderHouseLock::writeDone() {
	std::unique_lock<mutex> locker(myMutex);
    myWriter = false;
    myWriterSignal.notify_all();
}


string OrderHouse::addOrder(const Order& order) {
	myLocker.writeWait();
	string exchangeId = nextExchangeId();
	myOrderBook.emplace(exchangeId, Order(order.getClientId(), order.getInstrument(), order.getSide(),
			order.getPrice(), order.getQuantity(), exchangeId, OrderStatus_t::newAck_e));
	myLocker.writeDone();

	cout << "OrderHouse add order succeed for order " + exchangeId << endl;
	return exchangeId;
}

void OrderHouse::executeOrder(const string& exchangeId, int execQuantity) {
	myLocker.writeWait();
	auto itr = myOrderBook.find(exchangeId);
	if(itr == myOrderBook.end() ) {
		myLocker.writeDone();
		cout << "OrderHouse execute fails because of failure to find order " + exchangeId << endl;
		return;
	}
	//whether have enough quantity to execute
	if (itr->second.getQuantity() - itr->second.getQuantityDone() < execQuantity) {
		myLocker.writeDone();
		cout << "OrderHouse execute fails because of not enought quantity " << execQuantity << " for order: " + exchangeId << endl;
		return;
	}

	itr->second.setQuantityDone(itr->second.getQuantityDone() + execQuantity);
	itr->second.setStatus(OrderStatus_t::execution_e);

	myLocker.writeDone();
	cout << "OrderHouse execute order " + exchangeId << " with quantity " << execQuantity << endl;

}

void OrderHouse::cancelOrder (const string& exchangeId) {
	myLocker.writeWait();

	auto itr = myOrderBook.find(exchangeId);
	if(itr == myOrderBook.end() ) {
		myLocker.writeDone();
		cout << "OrderHouse cancel reject because of failure to find order " + exchangeId << endl;
		return;
	}

	if ( itr->second.getStatus() == OrderStatus_t::newAck_e ||
		 (itr->second.getStatus() == OrderStatus_t::execution_e && itr->second.getQuantity() > itr->second.getQuantityDone())) {
		itr->second.setStatus(OrderStatus_t::cancelAck_e);
		cout << "OrderHouse cancel succeed for order " + exchangeId << endl;
	} else {
		cout << "OrderHouse cancel reject because of current status " << (int)itr->second.getStatus() << endl;
		itr->second.setStatus(OrderStatus_t::cancelReject_e);
	}
	myLocker.writeDone();
}

OrderEvent OrderHouse::queryOrder(const string& exchangeId) {
	myLocker.readWait();
	auto itr = myOrderBook.find(exchangeId);
	if(itr != myOrderBook.end() ) {
		OrderEvent event(itr->second.getClientId(),
				          itr->second.getExchangeId(),
						  itr->second.getStatus(),
						  itr->second.getQuantityDone());
		myLocker.readDone();
		return event;
	} else {
		cout << "OrderHouse query order fails for idstatus " << (int)itr->second.getStatus() << endl;
		myLocker.readDone();
		return OrderEvent("",exchangeId,OrderStatus_t::unknown_e,0);
	}
}

std::string OrderHouse::nextExchangeId() {
	static int exchangeId = 0;
	std::ostringstream ss;
	ss<< std::setfill('0') << std::setw(9) << ++exchangeId;
	return ss.str();
}
