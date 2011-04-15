/*
 * irl_cc.h
 *
 *  Created on: Apr 4, 2011
 *      Author: mlchan
 */

#ifndef IRL_CC_H_
#define IRL_CC_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define IRL_IPV6_ADDRESS_LENGTH	16


#define FAC_CC_DISCONNECT			0
#define FAC_CC_RESERVED_OPCODE1		1
#define FAC_CC_RESERVED_OPCODE2		2
#define FAC_CC_REDIRECT_TO_SDA		3
#define FAC_CC_REDIRECT_REPORT		4
#define FAC_CC_CONNECTION_REPORT	5

#define	FAC_CC_CLIENTTYPE_SIM		0
#define	FAC_CC_CLIENTTYPE_DEVICE	1


int irlEnable_ConnectionControl(unsigned long irl_handle);

#ifdef __cplusplus
}
#endif

#endif /* IRL_CC_H_ */
