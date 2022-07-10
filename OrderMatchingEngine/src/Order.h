/*
 * Order.h
 *
 *  Created on: 2022Äê7ÔÂ9ÈÕ
 *      Author: yufei
 */

#ifndef ORDER_H_
#define ORDER_H_

#include <string>

namespace ENGINE {

enum class OrderSide_t {
	 unknown_e =0,
	 buy_e = 1,
     sell_e = 2
};

enum class OrderStatus_t {
	 unknown_e = 0,
     newAck_e,
     newReject_e,
	 cancelAck_e,
	 cancelReject_e,
	 execution_e
};

class Order {
public:
	Order(const std::string& clientId, int instrument, OrderSide_t side=OrderSide_t::unknown_e,  int price=0, int quantity=0,
		  const std::string& exchangeId ="", OrderStatus_t status = OrderStatus_t::unknown_e );
	Order() = delete;

	virtual ~Order() = default;

	//getter and setters
	std::string getClientId() const;
	void setClientId(const std::string& clientId);

	std::string getExchangeId() const;
	void setExchangeId(const std::string& exchangeId);

	OrderSide_t getSide() const;
	void setSide(OrderSide_t side);

	OrderStatus_t getStatus() const;
	void setStatus(OrderStatus_t status);

	int getInstrument() const;
	void setInstrument(int instrument);

	int getPrice() const;
	void setPrice(int price);

	int getQuantity() const;
	void setQuantity(int quantity);

	int getQuantityDone() const;
	void setQuantityDone(int quantityDone);

private:
	std::string myClientId;    //order reference issued by client
	int myInstrument; //a four digit number to represent securities on Jpx
	OrderSide_t mySide;
	int myPrice; //YEN, 0 represent market order
	int myQuantity;
	std::string myExchangeId;  //order reference issued by exchange
	OrderStatus_t myStatus;
	int myQuantityDone;
};

}
#endif /* ORDER_H_ */
