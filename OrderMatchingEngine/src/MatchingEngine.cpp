/*
 * MatchingEngine.cpp
 *
 *  Created on: 2022Äê7ÔÂ9ÈÕ
 *      Author: yufei
 */
#include <iostream>

#include "MatchingEngine.h"

using namespace std;
using namespace ENGINE;

int MatchingEngine::ourNumberOfBoards = 5;

void MatchingEngine::setNumberOfBoards(int boards) {
	MatchingEngine::ourNumberOfBoards = boards;
}

MatchingEngine& MatchingEngine::getInstance() {
	static MatchingEngine instance;
	return instance;
}

MatchingEngine::MatchingEngine():myBoards(ourNumberOfBoards) {
}

bool MatchingEngine::submitNew(const Order& order) {
	if(validationNew(order)) {
		myBoards[order.getInstrument()%ourNumberOfBoards].submitNew(order);
		return true;
	} else {
		return false;
	}
}

bool MatchingEngine::submitCancel(const Order& order) {
	if(validationCancel(order)) {
		myBoards[order.getInstrument()%ourNumberOfBoards].submitCancel(order);
		return true;
	} else {
		return false;
	}
}

OrderEvent MatchingEngine::getResultsOfLastOperation(const Order& order) {
	return myBoards[order.getInstrument()%ourNumberOfBoards].getResultsOfLastOperation(order);
}

bool MatchingEngine::validationNew(const Order& order) {
	if(order.getClientId().empty() || order.getInstrument() < 1000 ||
	   order.getQuantity() <=0 ||
	   order.getSide() == OrderSide_t::unknown_e) {
	   cout << "validation failed for new order " << order.getClientId() << endl;
	   return false;
	  } else {
	   return true;
	}
}

bool MatchingEngine::validationCancel(const Order& order) {
   if(order.getClientId().empty() || order.getInstrument() < 1000) {
	   cout << " validation failed for cancel order " << order.getClientId() << endl;
	   return false;
   } else {
	   return true;
   }
}
