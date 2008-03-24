//---------------------------------------------------------------------------
// Public declarations for the exported interface

void BIOS_Reset(void);
Can_Return_t BIOS_CanSend(Can_Message_t* msg);
void (*BIOS_CanCallback)(Can_Message_t *msg);
