/* Copyright © 2018-2019 Pascal JEAN, All rights reserved.
 * This file is part of the libmodbuspp Library.
 *
 * The libmodbuspp Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * The libmodbuspp Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the libmodbuspp Library; if not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <modbuspp/global.h>
#include <modbuspp/data.h>
#include <modbuspp/timeout.h>

namespace Modbus {
  
  class RtuLayer;
  class TcpLayer;

  /**
   * @class Device
   * @brief Device connected to Modbus
   *
   * This class is the base class for Master and Slave.
   * It groups together their common properties and methods.
   * 
   * @author Pascal JEAN, aka epsilonrt
   * @copyright GNU Lesser General Public License
   */
  class Device  {
    public:

      /**
       * @brief Constructor
       *
       * Constructs a Modbus device for the \b net network.
       *
       * For the Tcp backend :
       * - \b connection specifies the host name or IP
       * address of the host to connect to, eg. "192.168.0.5" , "::1" or
       * "server.com". A NULL value can be used to listen any addresses in server mode,
       * - \b settings is the service name/port number to connect to.
       * To use the default Modbus port use the string "502". On many Unix
       * systems, it’s convenient to use a port number greater than or equal
       * to 1024 because it’s not necessary to have administrator privileges.
       * .
       *
       * For the Rtu backend :
       * - \b connection specifies the name of the serial port handled by the
       *  OS, eg. "/dev/ttyS0" or "/dev/ttyUSB0",
       * - \b settings specifies communication settings as a string in the
       *  format BBBBPS. BBBB specifies the baud rate of the communication, PS
       *  specifies the parity and the bits of stop. \n
       *  According to Modbus RTU specifications :
       *    - the possible combinations for PS are E1, O1 and N2.
       *    - the number of bits of data must be 8, also there is no possibility
       *      to change this setting
       *    .
       * .
       *
       * An exception std::invalid_argument is thrown if one of the parameters
       * is incorrect.
       */
      Device (Net net = Tcp, const std::string & connection = "*",
              const std::string & settings = "502");

      /**
       * @brief Copy constructor
       */
      Device (const Device & other);

      /**
       * @brief Destructor
       *
       * The destructor closes the connection if it is open and releases all
       * affected resources.
       */
      virtual ~Device();

      /**
       * @brief Swaps the managed objects and associated deleters of *this and another
       */
      void swap (Device &other);

      /**
       * @brief Copy assignment operator
       */
      Device& operator= (const Device &other);

      /**
       * @brief Establish a Modbus connection
       *
       * This function shall establish a connection to a Modbus server,
       * a network or a bus
       *
       * @return true if successful.
       * Otherwise it shall return -1 and set errno.
       */
      virtual bool open();

      /**
       * @brief Close a Modbus connection
       */
      virtual void close();

      /**
       * @brief Returns true if the device is open; otherwise returns false.
       * @sa open()
       */
      bool isOpen() const;

      /**
       * @brief Flush non-transmitted data
       *
       * This function shall discard data received but not read to the socket or
       * file descriptor associated to this connection.
       *
       * @return 0 or the number of flushed bytes if successful.
       * Otherwise it shall return -1 and set errno.
       */
      int flush();

      /**
       * @brief Get slave number
       *
       * This function shall get the slave number.
       *
       * @return the slave number if successful.
       * Otherwise it shall return -1 and set errno
       * @sa setSlave()
       */
      int slave() const;

      /**
       * @brief Set slave number
       *
       * This function shall set the slave number with the \b id value.
       *
       * The behavior depends of network and the role of the device:
       * - \b RTU: Define the slave ID of the remote device to talk in master
       *  mode or set the internal slave ID in slave mode. According to the
       *  protocol, a Modbus device must only accept message holding its slave
       *  number or the special broadcast number.
       * - \b TCP: The slave number is only required in TCP if the message must
       *  reach a device on a serial network. Some not compliant devices or
       *  software (such as modpoll) uses the slave ID as unit identifier,
       *  that's incorrect (cf page 23 of Modbus Messaging Implementation
       *  Guide v1.0b) but without the slave value, the faulty remote device or
       *  software drops the requests ! \n
       *  The special value \b MODBUS_TCP_SLAVE (0xFF) can be used in TCP mode
       *  to restore the default value.
       * .
       *
       * The broadcast address is \b MODBUS_BROADCAST_ADDRESS. This special
       * value must be use when you want all Modbus devices of the network
       * receive the request.
       *
       * @return true if successful.
       * Otherwise it shall return false and set errno.
       * @sa slave()
       */
      bool setSlave (int id);

      /**
       * @brief Set timeout for response
       *
       * This function shall set the timeout interval used to wait for
       * a response. When a byte timeout is set, if elapsed time for the first
       * byte of response is longer than the given timeout, an `ETIMEDOUT` error
       * will be raised by the function waiting for a response. When byte
       * timeout is disabled, the full confirmation response must be received
       * before expiration of the response timeout.
       *
       * @param timeout reference on the variable that will contain the new 
       * timeout in seconds.
       * @sa responseTimeout()
       */
      void setResponseTimeout (const double & timeout);

      /**
       * @brief Get timeout for response
       *
       * @return timeout for response in seconds.
       * @sa setResponseTimeout()
       */
      double responseTimeout();

      /**
       * @brief Set timeout for response
       *
       * This function shall set the timeout interval used to wait for
       * a response. When a byte timeout is set, if elapsed time for the first
       * byte of response is longer than the given timeout, an `ETIMEDOUT` error
       * will be raised by the function waiting for a response. When byte
       * timeout is disabled, the full confirmation response must be received
       * before expiration of the response timeout.
       *
       * @param timeout reference on the variable that will contain the new timeout value
       * @sa responseTimeout()
       */
      void setResponseTimeout (const Timeout & timeout);

      /**
       * @brief Get timeout for response
       *
       * This function shall return the timeout interval used to wait
       * for a response in the \b timeout argument.
       * @param timeout reference on the variable that will contain the timeout value
       * @sa setResponseTimeout()
       */
      void responseTimeout (Timeout & timeout);

      /**
       * @brief Set timeout between bytes
       *
       * This function shall set the timeout interval between two consecutive
       * bytes of the same message. The timeout is an upper bound on the amount
       * of time elapsed before *select()* returns, if the time elapsed is longer
       * than the defined timeout, an `ETIMEDOUT` error will be raised by the
       * function waiting for a response.
       *
       * @param timeout reference on the variable that will contain the new 
       * timeout in seconds.
       * @sa byteTimeout()
       */
      void setByteTimeout (const double & timeout);

      /**
       * @brief Get timeout between bytes
       *
       * @return timeout between bytes in seconds.
       * @sa setByteTimeout()
       */
      double byteTimeout();

      /**
       * @brief Set timeout between bytes
       *
       * This function shall set the timeout interval between two consecutive
       * bytes of the same message. The timeout is an upper bound on the amount
       * of time elapsed before *select()* returns, if the time elapsed is longer
       * than the defined timeout, an `ETIMEDOUT` error will be raised by the
       * function waiting for a response.
       *
       * @param timeout reference on the variable that will contain the new timeout value
       * @sa byteTimeout()
       */
      void setByteTimeout (const Timeout & timeout);

      /**
       * @brief Get timeout between bytes
       *
       * This function shall store the timeout interval between two consecutive
       * bytes of the same message in the \b timeout argument.
       *
       * @param timeout reference on the variable that will contain the timeout value
       * @sa setByteTimeout()
       */
      void byteTimeout (Timeout & timeout);

      /**
       * @brief Modbus addressing mode
       *
       * This function shall return the Modbus addressing mode used.
       * The address mode used is, by default, that of the data model, that is
       * to say, a numbering of the registers from 1 to n.
       *
       * The Modbus application protocol defines precisely PDU addressing rules.
       * In a Modbus PDU each data is addressed from 0 to 65535.
       *
       * It also defines clearly a Modbus data model composed of 4 blocks that
       * comprises several elements numbered from 1 to n.
       *
       * In the Modbus data Model each element within a data block is numbered
       * from 1 to n.
       *
       * Afterwards the Modbus data model has to be bound to the device
       * application (IEC -61131 object, or other application model).
       *
       * @return true for Modbus PDU adressing
       * @sa setPduAdressing()
       */
      bool pduAdressing() const;

      /**
       * @brief Set the Modbus addressing mode
       *
       * @param pduAdressing true for Modbus PDU adressing
       * @sa pduAdressing()
       */
      void setPduAdressing (bool pduAdressing = true);

      /**
       * @brief Underlying layer used (backend)
       *
       * This function allows to know the underlying layer used.
       */
      Net net() const;

      /**
       * @brief underlying RTU layer (backend)
       *
       * This function shall return the RTU layer if it is the layer used by
       * the device. If it does not, a \b std::domain_error exception is thrown.
       */
      RtuLayer & rtu();

      /**
       * @brief underlying TCP layer (backend)
       *
       * This function shall return the TCP layer if it is the layer used by
       * the device. If it does not, a \b std::domain_error exception is thrown.
       */
      TcpLayer & tcp();

      /**
       * @brief last error message
       *
       * This function shall return the error message corresponding to
       * the last error. This function must be called right after the
       * instruction that triggered an error.
       */
      static std::string lastError();

      /**
       * @brief Set debug flag
       *
       * This function function shall set the debug flag by using the argument
       * \b flag. By default, the boolean flag is set to false.
       * When the \b flag value is set to true, many verbose messages are
       * displayed on stdout and stderr.
       * For example, this flag is useful to display the bytes of the
       * Modbus messages :
       * @code
       * [00][14][00][00][00][06][12][03][00][6B][00][03]
       * Waiting for a confirmation...
       * <00><14><00><00><00><09><12><03><06><02><2B><00><00><00><00>
       * @endcode
       *
       * @return return true if successful.
       * Otherwise it shall return false and set errno.
       */
      bool setDebug (bool debug = true);


      /**
       * @brief Read many discrete inputs (input bits)
       *
       * This function shall read the content of the \b nb input bits to the
       * address \b addr of the device.  The result of reading is stored
       * in \b dest array as  boolean.
       *
       * The function uses the Modbus function code 0x02 (read input status).
       *
       * @return the number of read input status if successful.
       * Otherwise it shall return -1 and set errno.
       */
      virtual int readDiscreteInputs (int addr, bool * dest, int nb = 1);

      /**
       * @brief Read many coils (bits)
       *
       * This function shall read the status of the \b nb bits (coils) to the
       * address \b addr of the device. The result of reading is stored
       * in \b dest array as boolean.
       *
       * The function uses the Modbus function code 0x01 (read coil status).
       *
       * @return the number of read bits if successful.
       * Otherwise it shall return -1 and set errno.
       */
      virtual int readCoils (int addr, bool * dest, int nb = 1);

      /**
       * @brief Write a single coil (bit)
       *
       * This function shall write the status of src at the address addr of
       * the device.
       *
       * The function uses the Modbus function code 0x05 (force single coil).
       *
       * @return 1 if successful.
       * Otherwise it shall return -1 and set errno.
       */
      virtual int writeCoil (int addr, bool src);

      /**
       * @brief Write many coils (bits)
       *
       * This function shall write the status of the \b nb bits (coils) from
       * \b src at the address \b addr of the device.
       * The \b src array must contains booelans.
       *
       * The function uses the Modbus function code 0x0F (force multiple coils).
       *
       * @return the number of written bits if successful.
       * Otherwise it shall return -1 and set errno.
       */
      virtual int writeCoils (int addr, const bool * src, int nb);

      /**
       * @brief Read many input registers
       *
       * This function shall read the content of the \b nb input registers to
       * the address \b addr of the device.
       *
       * The result of reading is stored in \b dest array as word values (16 bits).
       *
       * The function uses the Modbus function code 0x04 (read input registers).
       * The holding registers and input registers have different historical
       * meaning, but nowadays it's more common to use holding registers only.
       *
       * @return return the number of read input registers if successful.
       * Otherwise it shall return -1 and set errno.
       */
      virtual int readInputRegisters (int addr, uint16_t * dest, int nb = 1);

      /**
       * @brief Read many registers
       *
       * This function shall read the content of the nb holding registers to
       * the address \b addr of the device.
       *
       * The result of reading is stored in \b dest array as word values (16 bits).
       *
       * The function uses the Modbus function code 0x03 (read holding registers).
       *
       * @return return the number of read registers if successful.
       * Otherwise it shall return -1 and set errno.
       */
      virtual int readRegisters (int addr, uint16_t * dest, int nb = 1);

      /**
       * @brief Write a single register
       *
       * This function shall write the value of \b value holding registers at
       * the address \b addr of the device.
       *
       * The function uses the Modbus function code 0x06 (preset single register).
       *
       * @return 1 if successful.
       * Otherwise it shall return -1 and set errno.
       */
      virtual int writeRegister (int addr, uint16_t value);

      /**
       * @brief Write many registers
       *
       * This function shall write the content of the \b nb holding registers
       * from the array \b src at address \b addr of the device.
       *
       * The function uses the Modbus function code 0x10 (preset multiple registers).
       *
       * @return number of written registers if successful.
       * Otherwise it shall return -1 and set errno.
       */
      virtual int writeRegisters (int addr, const uint16_t * src, int nb);

      /**
       * @brief Read a single discrete input (input bit)
       *
       * This function shall read a single input bits to the address \b addr
       * of the device.  The result of reading is stored in \b dest
       * as boolean.
       *
       * The function uses the Modbus function code 0x02 (read input status).
       *
       * @return 1 if successful.
       * Otherwise it shall return -1 and set errno.
       */
      inline int readDiscreteInput (int addr, bool & dest) {

        return readDiscreteInputs (addr, &dest, 1);
      }

      /**
       * @brief Read a single coil (bit)
       *
       * This function shall read a signle bit (coil) to the address \b addr of
       * the device. The result of reading is stored in \b dest
       * as boolean.
       *
       * The function uses the Modbus function code 0x01 (read coil status).
       *
       * @return 1 if successful.
       * Otherwise it shall return -1 and set errno.
       */
      inline int readCoil (int addr, bool & dest) {

        return readCoils (addr, &dest, 1);
      }

      /**
       * @brief Read a single input register
       *
       * This function shall read a single input register to the address \b addr
       * of the device.
       *
       * The result of reading is stored in \b dest as word values (16 bits).
       *
       * The function uses the Modbus function code 0x04 (read input registers).
       * The holding registers and input registers have different historical
       * meaning, but nowadays it's more common to use holding registers only.
       *
       * @return 1 if successful.
       * Otherwise it shall return -1 and set errno.
       */
      inline int readInputRegister (int addr, uint16_t & dest) {

        return readInputRegisters (addr, &dest, 1);
      }

      /**
       * @brief Read a single register
       *
       * This function shall read a signle holding register to
       * the address \b addr of the device.
       *
       * The result of reading is stored in \b dest as word values (16 bits).
       *
       * The function uses the Modbus function code 0x03 (read holding registers).
       *
       * @return 1 if successful.
       * Otherwise it shall return -1 and set errno.
       */
      inline int readRegister (int addr, uint16_t & dest) {

        return readRegisters (addr, &dest, 1);
      }

      /**
       * @brief Read many input data
       *
       * Data is a template class for storing, transmitting, and receiving
       * arithmetic data in multiple 16-bit Modbus registers.
       *
       * This function shall read the content of the \b nb input data to
       * the address \b addr of the device.
       *
       * The result of reading is stored in \b dest array as \b T values.
       *
       * The function uses the Modbus function code 0x04 (read input registers).
       *
       * @return return the number of read input Modbus registers (16-bit) if successful.
       * Otherwise it shall return -1 and set errno.
       */
      template <typename T, Endian e> int readInputRegisters (int addr, Data<T, e> * dest, int nb = 1) {
        int ret;
        std::vector<uint16_t> buf (nb * dest[0].registers().size(), 0);

        ret = readInputRegisters (addr, buf.data(), buf.size());
        if (static_cast<std::size_t>(ret) == buf.size()) {
          int n = 0;
          
          for (int i = 0; i < nb; i++) {
            
            for (auto & r : dest[i].registers()) {
              
              r = buf[n++];
            }
            dest[i].updateValue();
          }
        }
        return ret;
      }

      /**
       * @brief Read a single input data
       *
       * Data is a template class for storing, transmitting, and receiving
       * arithmetic data in multiple 16-bit Modbus registers.
       *
       * This function shall read a single input data to the address \b addr
       * of the device.
       *
       * The result of reading is stored in \b dest as T value.
       *
       * The function uses the Modbus function code 0x04 (read input registers).
       *
       * @return return the number of read input Modbus registers (16-bit) if successful.
       * Otherwise it shall return -1 and set errno.
       */
      template <typename T, Endian e> int readInputRegister (int addr, Data<T, e> & dest) {
        int s = dest.registers().size();

        int r = readInputRegisters (addr, dest.registers().data(), s);
        if (r == s) {

          dest.updateValue();
        }
        return r;
      }

      /**
       * @brief Read a single holding data
       *
       * Data is a template class for storing, transmitting, and receiving
       * arithmetic data in multiple 16-bit Modbus registers.
       *
       * This function shall read a single holding data to the address \b addr
       * of the device.
       *
       * The result of reading is stored in \b dest as T value.
       *
       * The function uses the Modbus function code 0x03 (read holding registers).
       *
       * @return return the number of read holding Modbus registers (16-bit) if successful.
       * Otherwise it shall return -1 and set errno.
       */
      template <typename T, Endian e> int readRegister (int addr, Data<T, e> & dest) {
        int s = dest.registers().size();

        int r = readRegisters (addr, dest.registers().data(), s);
        if (r == s) {

          dest.updateValue();
        }
        return r;
      }

      /**
       * @brief Read many holding data
       *
       * Data is a template class for storing, transmitting, and receiving
       * arithmetic data in multiple 16-bit Modbus registers.
       *
       * This function shall read the content of the \b nb data to
       * the address \b addr of the device.
       *
       * The result of reading is stored in \b dest array as \b T values.
       *
       * The function uses the Modbus function code 0x03 (read holding registers).
       *
       * @return return the number of read holding Modbus registers (16-bit) if successful.
       * Otherwise it shall return -1 and set errno.
       */
      template <typename T, Endian e> int readRegisters (int addr, Data<T, e> * dest, int nb = 1) {
        int ret;
        std::vector<uint16_t> buf (nb * dest[0].registers().size(), 0);

        ret = readRegisters (addr, buf.data(), buf.size());
        if (static_cast<std::size_t>(ret) == buf.size()) {
          int n = 0;
          
          for (int i = 0; i < nb; i++) {
            
            for (auto & r : dest[i].registers()) {
              
              r = buf[n++];
            }
            dest[i].updateValue();
          }
        }
        return ret;
      }

      /**
       * @brief Write many holding data
       *
       * Data is a template class for storing, transmitting, and receiving
       * arithmetic data in multiple 16-bit Modbus registers.
       *
       * This function shall write the content of the \b nb holding data
       * from the array \b src at address \b addr of the device.
       *
       * The function uses the Modbus function code 0x10 (preset multiple registers).
       *
       * @return number of written holding Modbus registers (16-bit) if successful.
       * Otherwise it shall return -1 and set errno.
       */
      template <typename T, Endian e> int writeRegisters (int addr, const Data<T, e> * src, int nb = 1) {
        std::vector<uint16_t> buf;

        for (int i = 0; i < nb; i++) {
          for (auto & r : src[i].registers()) {
            buf.push_back (r);
          }
        }
        return writeRegisters (addr, buf.data(), buf.size());;
      }

      /**
       * @brief Write a single holding data
       *
       * Data is a template class for storing, transmitting, and receiving
       * arithmetic data in multiple 16-bit Modbus registers.
       *
       * This function shall write a single holding data
       * from \b value at address \b addr of the device.
       *
       * The function uses the Modbus function code 0x10 (preset multiple registers).
       *
       * @return number of written holding Modbus registers (16-bit) if successful.
       * Otherwise it shall return -1 and set errno.
       */
      template <typename T, Endian e> int writeRegister (int addr, const Data<T, e> & value) {

        return writeRegisters (addr, value.registers().data(), value.registers().size());
      }

      friend class RtuLayer;
      friend class TcpLayer;

    protected:
      class Private;
      Device (Private &dd);
      std::unique_ptr<Private> d_ptr;

    private:
      PIMP_DECLARE_PRIVATE (Device)
  };
}

/* ========================================================================== */
