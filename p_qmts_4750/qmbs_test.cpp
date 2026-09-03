/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2021-04-28 15:08:16
Description: service模板
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2F_ENTERACE(qmbs_test)
int f_tran_json_func(EIClass* blks_in, EIClass * blks_out, CDbConnection* conn);
int f_qmbs_test(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	EIClass iplat_Tab;
	try
	{
		iplat_Tab.Tables.Clear();
		iplat_Tab.Tables.Add();
		iplat_Tab.Tables[0].Columns.Add(DT_STRING, "JSON");
		iplat_Tab.Tables[0].Columns.Add(DT_STRING, "URL");
		iplat_Tab.Tables[0].Columns.Add(DT_STRING, "COL");
		iplat_Tab.Tables[0].Rows.Add();
		//产品构成
		//iplat_Tab.Tables[0].Rows[0][0] = "{\"date\":\"202306\"}";
		////"{\"modelId\":\"TGMX0008\",\"startTime\":\"20220101\",\"endTime\":\"20220901\",\"" + unit_code + "\"}";
		//Log::Trace("", "", "-----------------执行最新程序的识别-221-------------");
		//iplat_Tab.Tables[0].Rows[0]["URL"] = "http://eplat.tisco.com.cn/tgjy/service/S_SC_22";
		//iplat_Tab.Tables[0].Rows[0]["COL"] = "MXCPData";
		//原料预测
		iplat_Tab.Tables[0].Rows[0][0] = "{\"date\":\"20230717\"}";
		Log::Trace("", "", "-----------------执行最新程序的识别-221-------------");
		iplat_Tab.Tables[0].Rows[0]["URL"] = "http://eplat.tisco.com.cn/tgjy/service/S_SC_21";
		iplat_Tab.Tables[0].Rows[0]["COL"] = "EJKData";
		
		for (size_t i = 0; i < bcls_ret->Tables.get_Count(); i++)
		{
			for (size_t j = 0; j < bcls_ret->Tables[i].Columns.get_Count(); j++)
			{
				for (size_t k = 0; k < bcls_ret->Tables[i].Rows.get_Count(); k++)
				{
					Log::Trace("", "", "列名 =[{0}],行名=[{1}],", bcls_ret->Tables[i].Columns[j].get_ColumnName(), bcls_ret->Tables[i].Rows[k][j].ToString());
				}
			}
		}
	}
	catch (CDbException &ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char *)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException &ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException &ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}
