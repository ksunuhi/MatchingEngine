/*
 * OrderEvent.cpp
 *
 *  Created on: 2022Äê7ÔÂ9ÈÕ
 *      Author: yufei
 */

#include "OrderEvent.h"


namespace ENGINE {

OrderEvent::OrderEvent(const std::string& clientId, const std::string& exchangeId,  OrderStatus_t status, int quantityDone)
:myClientId(clientId),
 myExchangeId(exchangeId),
 myStatus(status),
 myQuantityDone(quantityDone) {

}

std::string OrderEvent::getClientId() const {
	return myClientId;
}
void OrderEvent::setClientId(const std::string& clientId) {
	myClientId = clientId;
}

std::string OrderEvent::getExchangeId() const {
	return myExchangeId;
}

void OrderEvent::setExchangeId(const std::string& exchangeId) {
	myExchangeId = exchangeId;
}


OrderStatus_t OrderEvent::getStatus() const {
	return myStatus;
}

void OrderEvent::setStatus(OrderStatus_t status) {
	myStatus = status;
}

int OrderEvent::getQuantityDone() const {
	return myQuantityDone;
}

void OrderEvent::setQuantityDone(int quantityDone) {
	myQuantityDone = quantityDone;
}




} /* namespace ENGINE */
