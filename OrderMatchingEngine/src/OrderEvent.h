/*
 * OrderEvent.h
 *
 *  Created on: 2022Äê7ÔÂ9ÈÕ
 *      Author: yufei
 */

#ifndef ORDEREVENT_H_
#define ORDEREVENT_H_

#include <string>
#include "Order.h"

namespace ENGINE {

class OrderEvent {
public:
	OrderEvent(const std::string& clientId, const std::string& exchangeId,  OrderStatus_t status, int quantityDone);
	virtual ~OrderEvent() = default;

	//getter and setters
	std::string getClientId() const;
	void setClientId(const std::string& clientId);

	std::string getExchangeId() const;
	void setExchangeId(const std::string& exchangeId);

	OrderStatus_t getStatus() const;
	void setStatus(OrderStatus_t status);

	int getQuantityDone() const;
	void setQuantityDone(int quantityDone);
private:
	std::string myClientId;    //order reference issued by client
	std::string myExchangeId;  //order reference issued by exchange
	OrderStatus_t myStatus;
	int myQuantityDone; //accumulated execution quantity
};

} /* namespace ENGINE */

#endif /* ORDEREVENT_H_ */
