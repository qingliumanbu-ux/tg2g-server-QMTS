/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      wsl
Version:     1.0
Date:        2024-10-25 14:13:28
Description: 请求修约成分发送电文
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_t80rye_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CModel tqmts0rdr("TQMTS0RDR");
	EPEX epex;	
	CDbCommand cmd_sql(conn);

	try
	{
		CString epex_number = "T80RYE";
		Log::Trace("", __FUNCTION__, "111");
		for (size_t i = 0; i < bcls_rec->Tables[1].Rows.get_Count(); i++)
		{
			CString heat_no = bcls_rec->Tables[1].Rows[i]["HEAT_NO"].ToString().Trim();
			Log::Trace("", __FUNCTION__, "222");

			CString order_no = bcls_rec->Tables[1].Rows[i]["ORDER_NO"].ToString().Trim();
			Log::Trace("", __FUNCTION__, "333");

			Log::Trace("", __FUNCTION__, "HEAT_NO ORDER_NO[{0},{1}]  ", heat_no, order_no);
			if (epex.Initialize(epex_number) < 0)
			{
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//获取熔炼号
			tqmts0rdr["HEAT_NO"] = heat_no;
			tqmts0rdr["ORDER_NO"] = order_no;
			tqmts0rdr.Query("HEAT_NO,ORDER_NO,REC_CREATE_TIME");
			tqmts0rdr.TrimOrBlank();
			/*if (epex.SetValue("DEAL_FLAG", 0, deal_flag) < 0)
			{
			sprintf(s.msg, (const char*)epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
			}*/

			if (epex.SetValue(0, tqmts0rdr) < 0)
			{
				sprintf(s.msg, epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (epex.SendTele() < 0)
			{
				Log::Trace("", "", "Tele[{0}]", (const char*)epex.GetMsg());
				sprintf(s.msg, (const char*)epex.GetMsg());
				throw CApplicationException(-1, s.msg, log.Location);
			}
			epex.Uninitialize();
		}
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}


