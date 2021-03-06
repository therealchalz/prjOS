#include <prjOS/include/cli/commandExecutor.h>

int32_t myAtoi(char *str) {
	/*
	 * Taken from http://www.geeksforgeeks.org/write-your-own-atoi/
	 * Used because I was having trouble wiht the atio/strtol function
	 * from stdlib, and I think it was a reentrancy issue.  Until that
	 * is figured out, we use this function
	 */
	if (*str == NULL)
	return 0;

	int32_t res = 0;  // Initialize result
	int8_t sign = 1;  // Initialize sign as positive
	int8_t i = 0;  // Initialize index of first digit

	// If number is negative, then update sign
	if (str[0] == '-') {
		sign = -1;
		i++;  // Also update index of first digit
	}

	// Iterate through all digits of input string and update result
	for (; str[i] != '\0'; ++i) {
		if (!(str[i] >= '0' && str[i] <= '9'))
			return 0; // You may add some lines to write error message
		// to error stream
		res = res*10 + str[i] - '0';
	}

	// Return result with sign
	return sign*res;
}

static char isCommandOk(char* command) {
	uint16_t idx = 0;
	for (idx = 0; idx < MAX_LINE_LENGTH; idx++) {
		if (command[idx] == 0)
			return 1;
		if (command[idx] >= 32 && command[idx] <= 126)
			continue;
		else
			return 0;
	}
	return 0;
}

static void executeCommand(cli_internal_data_t* data, uint8_t* command) {
	if (!isCommandOk(command)) {
		return;
	}
	cli_engine(data, (char*)command);
}

static void initCli(cli_internal_data_t* data, uint32_t serialDriverTid, uint32_t serialUITid) {
	cli_init_data_t init_data;
	init_data.serialDriverTid = serialDriverTid;
	init_data.serialUITid = serialUITid;
	init_data.prefix[0] = 0;

	cli_init(&init_data, data);
}

static void cmdTasksInfo(uint32_t argc, uint32_t* argv) {
	MetaInfoRequest metaRequest;

	MetaTasksInfoResult tasksResult;
	metaRequest.requestType = META_REQUEST_TASKS_INFO;
	metaRequest.extendedResult = &tasksResult;
	prjMetaInfo(&metaRequest);

	//TODO: use proper outputting
	bwprintf("Tasks: %d/%d\n\r", tasksResult.numTasks, tasksResult.maxNumTasks);
	bwprintf("Micro tasks: %d/%d\n\r", tasksResult.numMicroTasks, tasksResult.maxMicroNumTasks);
	bwprintf("Task stack size: %d Microtask stack size: %d\n\r", tasksResult.taskDefaultStackSize, tasksResult.microTaskDefaultStackSize);
}

static void cmdTime(uint32_t argc, uint32_t* argv) {
	uint64_t time;
	prjGetMonotonicMicros(&time);

	bwprintf("Time: %lu us\n\r", time);
}

static void cmdTaskInfo(uint32_t argc, uint32_t* argv) {
	MetaInfoRequest metaRequest;
	task_id_t taskId;

	if (argc == 1) {
		taskId = prjGetTid();
	} else {
		/*
		//char asd[10];
		//strcpy( asd, "12345");
		//bwprintf("ARG: %s\n\r", (char*)argv[1]);
		//bwprintf("ARG: %s\n\r", asd);
		struct _reent rent;
		struct _reent* old = _impure_ptr;
		_impure_ptr = &rent;
		taskId = (task_id_t)_strtol_r(old, (char*)(argv[1]), NULL, 10);
		_impure_ptr = old;
		//taskId = atoi((char*)(argv[1]));
		bwprintf("Got param: %u, argc is %d, errno is %d\n\r", taskId, argc, rent._errno);
		*/

		taskId = myAtoi((char*)(argv[1]));
	}

	MetaTasksInfoResult tasksResult;
	metaRequest.requestType = META_REQUEST_TASKS_INFO;
	metaRequest.extendedResult = &tasksResult;
	prjMetaInfo(&metaRequest);

	metaRequest.requestType = META_REQUEST_TASK_INFO;
	MetaTaskInfoResult taskResult;
	metaRequest.extendedResult = &taskResult;
	metaRequest.taskId=taskId;

	bwprintf("Task iD: %d\n\r", taskId);

	prjMetaInfo(&metaRequest);

	uint32_t stackSize = tasksResult.taskDefaultStackSize;
	if (taskResult.taskType == TASK_TYPE_MICRO)
		stackSize = tasksResult.microTaskDefaultStackSize;

	//TODO: use proper outputting
	bwprintf("Task id: %d, Parent: %d\n\r", taskResult.taskID, taskResult.parentTid);
	bwprintf("Stack base: 0x%x Stack Head: 0x%x\n\r", taskResult.stackBase, taskResult.stackHead);
	bwprintf("Stack usage: %d bytes; %d%%\n\r", (taskResult.stackBase - taskResult.stackHead), ((taskResult.stackBase - taskResult.stackHead)*100/stackSize));
	bwprintf("Task State: %d\n\r", taskResult.taskState);
	bwprintf("Context switches: %lu\n\r", taskResult.contextSwitchCount);
	bwprintf("Task Time - User: %lu us\n\r", taskResult.userTimeMicros);
	bwprintf("Task Time - System: %lu us\n\r", taskResult.systemTimeMicros);

}

static void commandExecutorEntry(void) {
	uint32_t parentTid;
	serialUIMessage message;
	uint8_t command[MAX_LINE_LENGTH];

	CommandExecutorData data;

	prjReceive(&parentTid, (uint8_t*)&data, sizeof(CommandExecutorData));
	prjReply(parentTid, NULL, 0);

	//cli_record_t cmdRecordLight;
	cli_record_t cmdtaskInfo;
	cli_record_t cmdtasksInfo;
	cli_record_t cmdtimeInfo;

	cli_internal_data_t cliData;

	initCli(&cliData, data.serialDriverTid, data.serialUITid);
	//cli_mkcmd(&cliData, "light", (cli_funcvar_t)&cmdLight, 0, &cmdRecordLight);
	cli_mkcmd(&cliData, "task", (cli_funcvar_t)&cmdTaskInfo, NULL, &cmdtaskInfo);
	cli_mkcmd(&cliData, "tasks", (cli_funcvar_t)&cmdTasksInfo, NULL, &cmdtasksInfo);
	cli_mkcmd(&cliData, "time", (cli_funcvar_t)&cmdTime, NULL, &cmdtimeInfo);

	message.messageType = SERIAL_UI_MESSAGE_TYPE_COMMAND_SUBSCRIBE;

	while (1) {
		prjSend(data.serialUITid, (uint8_t*)&message, sizeof(message),
				&command, MAX_LINE_LENGTH);
		executeCommand(&cliData, command);
	}
	prjExit();
}

task_id_t cliInit(CliInitData *cliInitData) {
	task_id_t commandExecutorTid;
	uint32_t dummy;
	CommandExecutorData data;

	commandExecutorTid = prjCreate(0,commandExecutorEntry);

	bwprintf("Init: Command executor running (%d)\n\r", commandExecutorTid);

	data.serialUITid = cliInitData->serialUITid;
	data.serialDriverTid = cliInitData->serialDriverTid;

	prjSend(commandExecutorTid, (uint8_t*)&data, sizeof(CommandExecutorData), &dummy, 4);

	return commandExecutorTid;
}
