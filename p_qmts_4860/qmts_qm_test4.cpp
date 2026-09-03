/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      wsl
Version:     1.0
Date:        2024-05-27 16:59:33
Description: 质量损失4降级品
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2F_ENTERACE(qmts_qm_test4)
int f_tran_json_func(EIClass* blks_in, EIClass * blks_out, CDbConnection* conn);

int f_qmts_qm_test4(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CDbCommand cmd_inq(conn);
	EIClass iplat_Tab;
	CString dateS = CDateTime::Now().AddDays(-1).ToString("yyyyMMdd");
	CString dateE = CDateTime::Now().ToString("yyyyMMdd");
	CModel tqmtssc27("TQMTSSC27");
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{
		Log::Trace("", "", "-----------------执行程序-------------");
		iplat_Tab.Tables.Clear();
		iplat_Tab.Tables.Add();
		iplat_Tab.Tables[0].Columns.Add(DT_STRING, "JSON");
		iplat_Tab.Tables[0].Columns.Add(DT_STRING, "URL");
		iplat_Tab.Tables[0].Columns.Add(DT_STRING, "COL");
		iplat_Tab.Tables[0].Rows.Add();


		iplat_Tab.Tables[0].Rows[0][0] = "{\"date_s\":\"20240501\",\"date_e\":\"" + dateE + "\"}";

		Log::Trace("", "", "-----------------执行最新程序的识别-221-------------");
		iplat_Tab.Tables[0].Rows[0]["URL"] = "http://eplat.tisco.com.cn/tgjy/service/S_SC_27";
		iplat_Tab.Tables[0].Rows[0]["COL"] = "ZLSSJJPData";

		Log::Trace("", "", "-----------------调用sql代理-------------");

		doFlag = f_tran_json_func(&iplat_Tab, bcls_ret, conn);
		if (doFlag < 0)
		{
			Log::Trace("", "", "[{0}]", doFlag);
		}
		//删除
		sqlstr = "DELETE  FROM TQMTSSC27 WHERE 1=1";
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteNonQuery();
		cmd_inq.Close();
		for (int i = 0; i < bcls_ret->Tables[0].Rows.get_Count(); i++)
		{
			Log::Trace("", "", "--共[{0}]行，第[{1}]行数据---", bcls_ret->Tables[0].Rows.get_Count(), i);

			if (i > 15)
			{
				break;
			}
			for (int t = 0; t < bcls_ret->Tables[0].Columns.get_Count(); t++)
			{
				Log::Trace("", "", "--列名：[{0}],值：[{1}]---", bcls_ret->Tables[0].Columns[t].get_ColumnName(), bcls_ret->Tables[0].Rows[i][t].ToString());
			}
		}
		for (int i = 0; i < bcls_ret->Tables[0].Rows.get_Count(); i++)
		{
		tqmtssc27.Reset();
		tqmtssc27.MergeFrom(bcls_ret->Tables[0].Rows[i]);
		tqmtssc27.TrimOrBlank();//如果有Null
		tqmtssc27.Insert();
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