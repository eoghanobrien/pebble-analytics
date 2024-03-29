#include <pebble.h>
#include <messageresult.h>

void log_app_message_result(AppMessageResult app_message_error) {
  switch(app_message_error) {
      case APP_MSG_OK:
        APP_LOG(APP_LOG_LEVEL_INFO, "All good, operation was successful.");
        break;
      case APP_MSG_SEND_TIMEOUT:
        APP_LOG(APP_LOG_LEVEL_ERROR, "The other end did not confirm receiving the sent data with an (n)ack in time.");
        break;
      case APP_MSG_SEND_REJECTED:
        APP_LOG(APP_LOG_LEVEL_ERROR, "The other end rejected the sent data, with a 'nack' reply.");
        break;
      case APP_MSG_NOT_CONNECTED:
        APP_LOG(APP_LOG_LEVEL_ERROR, "The other end was not connected.");
        break;
      case APP_MSG_APP_NOT_RUNNING:
        APP_LOG(APP_LOG_LEVEL_ERROR, "The local application was not running.");
        break;
      case APP_MSG_INVALID_ARGS:
        APP_LOG(APP_LOG_LEVEL_ERROR, "The function was called with invalid arguments.");
        break;
      case APP_MSG_BUSY:
        APP_LOG(APP_LOG_LEVEL_ERROR, "There are pending (in or outbound) messages that need to be processed first before new ones can be received or sent.");
        break;
      case APP_MSG_BUFFER_OVERFLOW:
        APP_LOG(APP_LOG_LEVEL_ERROR, "The buffer was too small to contain the incoming message.");
        break;
      case APP_MSG_ALREADY_RELEASED:
        APP_LOG(APP_LOG_LEVEL_ERROR, "The resource had already been released.");
        break;
      case APP_MSG_CALLBACK_ALREADY_REGISTERED:
        APP_LOG(APP_LOG_LEVEL_ERROR, "The callback node was already registered, or its ListNode has not been initialized.");
        break;
      case APP_MSG_CALLBACK_NOT_REGISTERED:
        APP_LOG(APP_LOG_LEVEL_ERROR, "The callback could not be deregistered, because it had not been registered before.");
        break;
      case APP_MSG_OUT_OF_MEMORY:
        APP_LOG(APP_LOG_LEVEL_ERROR, "The support library did not have sufficient application memory to perform the requested operation.");
        break;
      case APP_MSG_CLOSED:
        APP_LOG(APP_LOG_LEVEL_ERROR, "App message was closed.");
        break;
      case APP_MSG_INTERNAL_ERROR:
        APP_LOG(APP_LOG_LEVEL_ERROR, "An internal OS error prevented APP_MSG from completing an operation");
        break;
    }
}