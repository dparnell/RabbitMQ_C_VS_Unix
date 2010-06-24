
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <stdarg.h>

#include "amqp.h"
#include "amqp_private.h"

#ifdef WIN32

#include <winsock2.h>

uint8_t buf_at(amqp_bytes_t bytes, int offset)
{
  return (uint8_t) bytes->bytes[offset];
}

uint16_t amqp_ntohs(uint8_t *buffer)
{
  uint16_t v;
  memcpy(&v, buffer, 2);

  return ntohs(v);
}

uint32_t amqp_ntohl(uint8_t *buffer)
{
  uint16_t v;
  memcpy(&v, buffer, 4);

  return ntohl(v);
}

int8_t amqp_d8(amqp_bytes_t bytes, uint16_t offset)
{
  if(( offset + 1 ) > bytes.len )
  {
    return -EFAULT; 
  }
  else
  {
    return buf_at( bytes, offset ); 
  }
}

int16_t amqp_d16(amqp_bytes_t bytes, uint16_t offset)
{
  if(( offset + 2 ) > bytes.len )
  {
    return -EFAULT; 
  }
  else
  {
    return amqp_ntohs( buf_at( bytes, offset )); 
  }
}

int32_t amqp_d32(amqp_bytes_t bytes, uint16_t offset)
{
  if(( offset + 4 ) > bytes.len )
  {
    return -EFAULT; 
  }
  else
  {
    return amqp_ntohl( buf_at( bytes, offset )); 
  }
}

int64_t amqp_d64(amqp_bytes_t bytes, uint16_t offset)
{
  uint64_t hi = amqp_d32(bytes, offset);
  uint64_t lo = amqp_d32(bytes, offset + 4);

  if( hi < 0 )
	return hi;

  if( lo < 0 )
	return lo;

  return hi << 32 | lo;
}

int8_t *amqp_dbytes(amqp_bytes_t bytes, uint16_t offset, uint16_t len)
{
  if(( offset + len ) > bytes.len )
  {
    return -EFAULT; 
  }
  else
  {
    return &buf_at( bytes, offset ); 
  }
}

void amqp_assert( bool condition, ... )
{ 
  va_list argp;

  va_start(argp, condition);

  if( ! condition )
  {
	fprintf(stderr, argp);
	fputc('\n', stderr);
	abort();
  }
  
  va_end( argp );
}

#endif
