#include "stdint.h"
typedef union _REG_CiFIFOCON {
    // Receive FIFO

    struct {
        uint32_t RxNotEmptyIE : 1;
        uint32_t RxHalfFullIE : 1;
        uint32_t RxFullIE : 1;
        uint32_t RxOverFlowIE : 1;
        uint32_t unimplemented1 : 1;
        uint32_t RxTimeStampEnable : 1;
        uint32_t unimplemented2 : 1;
        uint32_t TxEnable : 1;
        uint32_t UINC : 1;
        uint32_t unimplemented3 : 1;
        uint32_t FRESET : 1;
        uint32_t unimplemented4 : 13;
        uint32_t FifoSize : 5;
        uint32_t PayLoadSize : 3;
    } rxBF;

    // Transmit FIFO

    struct {
        uint32_t TxNotFullIE : 1;
        uint32_t TxHalfFullIE : 1;
        uint32_t TxEmptyIE : 1;
        uint32_t unimplemented1 : 1;
        uint32_t TxAttemptIE : 1;
        uint32_t unimplemented2 : 1;
        uint32_t RTREnable : 1;
        uint32_t TxEnable : 1;
        uint32_t UINC : 1;
        uint32_t TxRequest : 1;
        uint32_t FRESET : 1;
        uint32_t unimplemented3 : 5;
        uint32_t TxPriority : 5;
        uint32_t TxAttempts : 2;
        uint32_t unimplemented4 : 1;
        uint32_t FifoSize : 5;
        uint32_t PayLoadSize : 3;
    } txBF;
    uint32_t word;
    uint8_t byte[4];
} REG_CiFIFOCON;

int main(void)
{
    REG_CiFIFOCON v ;
    int x = sizeof(v);
    x;
    return 0;
}
