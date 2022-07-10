/*
 * Order.cpp
 *
 *  Created on: 2022Äê7ÔÂ9ÈÕ
 *      Author: yufei
 */

#include <string>
#include "Order.h"

using namespace std;
using namespace ENGINE;

Order::Order(const std::string& clientId, int instrument, OrderSide_t side, int price, int quantity,
		const std::string& exchangeId, OrderStatus_t status)
:myClientId(clientId),
 myInstrument(instrument),
 mySide(side),
 myPrice(price),
 myQuantity(quantity),
 myExchangeId(exchangeId),
 myStatus(status),
 myQuantityDone(0) {

}

std::string Order::getClientId() const {
	return myClientId;
}
void Order::setClientId(const std::string& clientId) {
	myClientId = clientId;
}

std::string Order::getExchangeId() const {
	return myExchangeId;
}

void Order::setExchangeId(const std::string& exchangeId) {
	myExchangeId = exchangeId;
}


OrderSide_t Order::getSide() const {
	return mySide;
}

void Order::setSide(OrderSide_t side) {
	mySide = side;
}

OrderStatus_t Order::getStatus() const {
	return myStatus;
}

void Order::setStatus(OrderStatus_t status) {
	myStatus = status;
}

int Order::getInstrument() const {
	return myInstrument;
}

void Order::setInstrument(int instrument) {
	myInstrument = instrument;
}

int Order::getPrice() const {
	return myPrice;
}

void Order::setPrice(int price) {
	myPrice = price;
}

int Order::getQuantity() const {
	return myQuantity;
}

void Order::setQuantity(int quantity) {
	myQuantity = quantity;
}

int Order::getQuantityDone() const {
	return myQuantityDone;
}

void Order::setQuantityDone(int quantityDone) {
	myQuantityDone = quantityDone;
}


