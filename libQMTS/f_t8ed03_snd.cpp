/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      wsl
Version:     1.0
Date:        2024-5-22 9:13:28
Description: 倒灌站铁水成分
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_t8ed03_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	EPEX epex;
	CDbCommand cmd_sql(conn);
	CModel tqmts24("TQMTS24");

	try
	{
		//初始化
		CString epex_number = "T8ED03";
		if (epex.Initialize(epex_number) < 0)
		{
			sprintf(s.msg, (const char*)epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//获取试样号
		tqmts24["ST_SAMPLE_NO"] = bcls_rec->Tables[0].Rows[0]["ST_SAMPLE_NO"];
		tqmts24.Query("ST_SAMPLE_NO");

		//处理SAMPLE_TAKEN_TIME为空时，导致的倒罐站电文堵塞问题
		if (tqmts24["SAMPLE_TAKEN_TIME"].ToString().Trim() == ""){
			tqmts24["SAMPLE_TAKEN_TIME"] = datetime;
		}
	
		//化验时间戳
		if (epex.SetValue("TIME_STAMP", 0, tqmts24["SAMPLE_TAKEN_TIME"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//序号
		if (epex.SetValue("ID", 0, tqmts24["ID_ELM"].ToDouble()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//样号
		if (epex.SetValue("SAMPLE_ID", 0, tqmts24["ST_SAMPLE_NO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//C
		if (epex.SetValue("C", 0, tqmts24["ELM_001"].ToDouble()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//SI
		if (epex.SetValue("SI", 0, tqmts24["ELM_002"].ToDouble()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//Mn
		if (epex.SetValue("Mn", 0, tqmts24["ELM_003"].ToDouble()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//P
		if (epex.SetValue("P", 0, tqmts24["ELM_004"].ToDouble()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//S
		if (epex.SetValue("S", 0, tqmts24["ELM_005"].ToDouble()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//CR
		if (epex.SetValue("CR", 0, tqmts24["ELM_006"].ToDouble()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//NI
		if (epex.SetValue("NI", 0, tqmts24["ELM_007"].ToDouble()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//CU
		if (epex.SetValue("CU", 0, tqmts24["ELM_009"].ToDouble()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//V
		if (epex.SetValue("V", 0, tqmts24["ELM_012"].ToDouble()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//TI
		if (epex.SetValue("TI", 0, tqmts24["ELM_013"].ToDouble()) < 0)
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


