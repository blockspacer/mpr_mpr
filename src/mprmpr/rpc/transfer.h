#ifndef KUDU_RPC_TRANSFER_H
#define KUDU_RPC_TRANSFER_H

#include <boost/intrusive/list.hpp>
#include <gflags/gflags.h>
#include <set>
#include <stdint.h>
#include <string>
#include <vector>

#include "mprmpr/rpc/constants.h"
#include "mprmpr/util/net/sockaddr.h"
#include "mprmpr/util/status.h"
#include "mprmpr/util/slice.h"

DECLARE_int32(rpc_max_message_size);

namespace google {
namespace protobuf {
class Message;
} // namespace protobuf
} // namespace google

namespace mprmpr {

class Socket;

namespace rpc {

class Messenger;
struct TransferCallbacks;

// This class is used internally by the RPC layer to represent an inbound
// transfer in progress.
//
// Inbound Transfer objects are created by a Connection receiving data. When the
// message is fully received, it is either parsed as a call, or a call response,
// and the InboundTransfer object itself is handed off.
class InboundTransfer {
 public:

  InboundTransfer();

  // read from the socket into our buffer
  Status ReceiveBuffer(Socket &socket);

  // Return true if any bytes have yet been sent.
  bool TransferStarted() const;

  // Return true if the entire transfer has been sent.
  bool TransferFinished() const;

  Slice data() const {
    return Slice(buf_);
  }

  // Return a string indicating the status of this transfer (number of bytes received, etc)
  // suitable for logging.
  std::string StatusAsString() const;

 private:

  Status ProcessInboundHeader();

  faststring buf_;

  int32_t total_length_;
  int32_t cur_offset_;

  DISALLOW_COPY_AND_ASSIGN(InboundTransfer);
};

// When the connection wants to send data, it creates an OutboundTransfer object
// to encompass it. This sits on a queue within the Connection, so that each time
// the Connection wakes up with a writable socket, it consumes more bytes off
// the next pending transfer in the queue.
//
// Upon completion of the transfer, a callback is triggered.
class OutboundTransfer : public boost::intrusive::list_base_hook<> {
 public:
  enum { kMaxPayloadSlices = 10 };

  // Factory methods for creating transfers associated with call requests
  // or responses. The 'payload' slices will be concatenated and
  // written to the socket. When the transfer completes or errors, the
  // appropriate method of 'callbacks' is invoked.
  //
  // Does not take ownership of the callbacks object or the underlying
  // memory of the slices. The slices must remain valid until the callback
  // is triggered.
  //
  // NOTE: 'payload' is currently restricted to a maximum of kMaxPayloadSlices
  // slices.
  // ------------------------------------------------------------

  // Create an outbound transfer for a call request.
  static OutboundTransfer* CreateForCallRequest(int32_t call_id,
                                                const std::vector<Slice> &payload,
                                                TransferCallbacks *callbacks);

  // Create an outbound transfer for a call response.
  // See above for details.
  static OutboundTransfer* CreateForCallResponse(const std::vector<Slice> &payload,
                                                 TransferCallbacks *callbacks);

  // Destruct the transfer. A transfer object should never be deallocated
  // before it has either (a) finished transferring, or (b) been Abort()ed.
  ~OutboundTransfer();

  // Abort the current transfer, with the given status.
  // This triggers TransferCallbacks::NotifyTransferAborted.
  void Abort(const Status &status);

  // send from our buffers into the sock
  Status SendBuffer(Socket &socket);

  // Return true if any bytes have yet been sent.
  bool TransferStarted() const;

  // Return true if the entire transfer has been sent.
  bool TransferFinished() const;

  // Return the total number of bytes to be sent (including those already sent)
  int32_t TotalLength() const;

  std::string HexDump() const;

  bool is_for_outbound_call() const {
    return call_id_ != kInvalidCallId;
  }

  // Returns the call ID for a transfer associated with an outbound
  // call. Must not be called for call responses.
  int32_t call_id() const {
    DCHECK_NE(call_id_, kInvalidCallId);
    return call_id_;
  }

 private:
  OutboundTransfer(int32_t call_id,
                   const std::vector<Slice> &payload,
                   TransferCallbacks *callbacks);

  // Slices to send. Uses an array here instead of a vector to avoid an expensive
  // vector construction (improved performance a couple percent).
  Slice payload_slices_[kMaxPayloadSlices];
  size_t n_payload_slices_;

  // The current slice that is being sent.
  int32_t cur_slice_idx_;
  // The number of bytes in the above slice which has already been sent.
  int32_t cur_offset_in_slice_;

  TransferCallbacks *callbacks_;

  // In the case of outbound calls, the associated call ID.
  // In the case of call responses, kInvalidCallId
  int32_t call_id_;

  bool aborted_;

  DISALLOW_COPY_AND_ASSIGN(OutboundTransfer);
};

// Callbacks made after a transfer completes.
struct TransferCallbacks {
 public:
  virtual ~TransferCallbacks();

  // The transfer finished successfully.
  virtual void NotifyTransferFinished() = 0;

  // The transfer was aborted (e.g because the connection died or an error occurred).
  virtual void NotifyTransferAborted(const Status &status) = 0;
};

} // namespace rpc
} // namespace kudu
#endif
