#ifndef RX_OPCODE_READER
#define RX_OPCODE_READER





class RXScriptOpcodeReaderEvent
{
public:
	//Event
	virtual void OnActiveUnderlay(unsigned short imageID, unsigned char xOffset, unsigned char yOffset){};
	virtual void OnActiveOverlay(unsigned short imageID, unsigned char xOffset, unsigned char yOffset){};
	virtual void OnSetFrame(unsigned short imageNumber){};
	virtual void OnUseParentFrame(){};
};



//Simple class that read an opcode flow until a "goto" is reached (we don't need more, as we are only aiming at reading
//the "initial" animation)

class RXScriptOpcodeReader
{
public:
	RXScriptOpcodeReader(void);
	~RXScriptOpcodeReader(void);

	void read(const unsigned char *, RXScriptOpcodeReaderEvent *eventDispatch);

private:
	const static int opcodeSize[];

	enum RXOpcode
	{
		//Opcode we search
		SET_FRAME = 0,
		SET_ACTIVE_OVERLAY = 8,
		SET_ACTIVE_UNDERLAY = 9,
		USE_PARENT_FRAME = 0x1D,

		//Opcode that will makes us stop reading the stream
		GOTO_OFFSET				=7,

		STOP_AND_REMOVEGRAPHICS = 0x16, 

		IFUNKNOWN_GOTO1			=0x35,
		IF_POWERUP_PICKED_GOTO	=0x39,
		IF_TARGET_IN_RANGE		=0x3A,
		IF_TARGET_ARC			=0x3B,
		IFUNKNOWN_GOTO2			=0x3F,
	};

	template<typename T, typename FUNCTION>
	void DispatchEvent(RXScriptOpcodeReaderEvent *event, FUNCTION func, const unsigned char *data);

	template<typename T, typename T2, typename FUNCTION>
	void DispatchEvent2(RXScriptOpcodeReaderEvent *event, FUNCTION func, const unsigned char *data);

	template<typename T, typename T2, typename T3,typename FUNCTION>
	void DispatchEvent3(RXScriptOpcodeReaderEvent *event, FUNCTION func, const unsigned char *data);



	static inline bool isGoto(RXOpcode opcode);
};

#endif

