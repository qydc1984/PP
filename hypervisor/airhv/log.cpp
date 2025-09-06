#define _NO_CRT_STDIO_INLINE
#include <ntifs.h>
#include <stdarg.h>
#include <ntstrsafe.h>
#include "log.h"

void LogPrint(__log_type type, const char* fmt, ...)
{
	const char* LogType = NULL;
	LARGE_INTEGER SystemTime = {};
	LARGE_INTEGER LocalTime = {};
	TIME_FIELDS TimeFields = {};
	char TimeBuffer[20] = {};
	char MessageBuffer[412] = {};
	const char* const OutputFormat = "%s  %s  %s\r\n";
	char OutputBuffer[512] = {};
	va_list Args = {};
	NTSTATUS Status;

	// 初始化 LogType 为 NULL，防止未定义行为
	LogType = NULL;

	switch (type)
	{
	case LOG_TYPE_DEBUG:
		LogType = "[DEBUG]";
		break;
	case LOG_TYPE_DUMP:
		LogType = "[DUMP]";
		break;
	case LOG_TYPE_ERROR:
		LogType = "[ERROR]";
		break;
	case LOG_TYPE_INFO:
		LogType = "[INFORMATION]";
		break;
	default:
		break;
	}

	// 如果 LogType 未设置，可以选择跳过日志或使用默认值
	if (LogType == NULL)
	{
		LogType = "[UNKNOWN]";
	}

	KeQuerySystemTimePrecise(&SystemTime);
	ExSystemTimeToLocalTime(&SystemTime, &LocalTime);
	RtlTimeToTimeFields(&LocalTime, &TimeFields);

	Status = RtlStringCchPrintfA(
		TimeBuffer,
		sizeof(TimeBuffer),
		"[%02hd:%02hd:%02hd.%03hd]",
		TimeFields.Hour,
		TimeFields.Minute,
		TimeFields.Second,
		TimeFields.Milliseconds);

	if (!NT_SUCCESS(Status))
	{
		return;
	}

	va_start(Args, fmt);
	Status = RtlStringCchVPrintfA(MessageBuffer, sizeof(MessageBuffer), fmt, Args);
	va_end(Args);

	if (!NT_SUCCESS(Status))
	{
		return;
	}

	Status = RtlStringCchPrintfA(
		OutputBuffer,
		sizeof(OutputBuffer),
		OutputFormat,
		TimeBuffer,
		LogType,
		MessageBuffer);

	if (!NT_SUCCESS(Status))
	{
		return;
	}

	ULONG DebugLevel = DPFLTR_ERROR_LEVEL;
	switch (type)
	{
	case LOG_TYPE_DEBUG:
		DebugLevel = DPFLTR_TRACE_LEVEL;
		break;
	case LOG_TYPE_INFO:
		DebugLevel = DPFLTR_INFO_LEVEL;
		break;
	case LOG_TYPE_ERROR:
		DebugLevel = DPFLTR_ERROR_LEVEL;
		break;
	default:
		DebugLevel = DPFLTR_MASK;
		break;
	}

	DbgPrintEx(DPFLTR_IHVDRIVER_ID, DebugLevel, "%s", OutputBuffer);
}