#if !defined( __AMQP_NONGNUC_H__ )
#define __AMQP_NONGNUC_H___

#if !defined( __GNUC__ )

#if defined( __cplusplus )
extern "C" {
#endif

#include <stdint.h>

#include "amqp.h"

/* FUNCTION DECLARATIONS */

uint8_t    buf_at(amqp_bytes_t bytes, int offset);
uint16_t   amqp_ntohs(uint8_t *buffer);
uint32_t   amqp_ntohl(uint8_t *buffer);
void       amqp_assert( int condition, ... );

int8_t     amqp_d8(amqp_bytes_t bytes, uint16_t offset);
int16_t    amqp_d16(amqp_bytes_t bytes, uint16_t offset);
int32_t    amqp_d32(amqp_bytes_t bytes, uint16_t offset);
int64_t    amqp_d64(amqp_bytes_t bytes, uint16_t offset);
int8_t    *amqp_dbytes(amqp_bytes_t bytes, uint16_t offset, uint16_t len);

#if defined( __cplusplus )
}
#endif

#endif /* __GNUC__ */

#endif /* __AMQP_NONGNUC_UTILS__ */