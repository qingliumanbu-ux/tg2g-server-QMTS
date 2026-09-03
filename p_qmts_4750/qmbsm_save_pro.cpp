/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      wsl
Version:     1.0
Date:        2021-04-28 15:08:16
Description: service模板
**************************************************/

#include "stdafx.h"

/* ***** 外部函数申明 ***** */
int f_qmts_grind_desc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//修磨要求生成

BM2F_ENTERACE(qmbsm_save_pro)
int f_qmbsm_save_pro(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	int seq_id = 0;
	CDbCommand cmd_inq(conn);
	CString  dateNow = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CModel tqmts11a("TQMTS11A");
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	try
	{
		if (bcls_rec->Tables.Contains("B_ADD") && bcls_rec->Tables.Contains("ROW_CODE"))
		{
			Log::Trace("", "", "新增开始1,count=[{0}]", bcls_rec->Tables["B_ADD"].Rows.get_Count());

			for (int i = 0; i < bcls_rec->Tables["B_ADD"].Rows.get_Count(); i++)
			{
				//查询序列号最大
				CString sqlstr1 = "SELECT MAX(SEQ_ID) FROM  TQMTS11A";
				cmd_inq.SetCommandText(sqlstr1);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					seq_id = cmd_inq.GetInt16(1);
				}
				cmd_inq.Close();
				tqmts11a.MergeFrom(bcls_rec->Tables["B_ADD"].Rows[i]);

				tqmts11a["REC_CREATOR"] = s.userid;
				tqmts11a["REC_CREATE_TIME"] = dateNow;
				tqmts11a["SEQ_ID"] = seq_id + 1;
				tqmts11a.Insert();

				//修磨要求生成
				doFlag = f_qmts_grind_desc(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					strcpy(s.msg, "调用函数报错!");
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
		}
		if (bcls_rec->Tables.Contains("B_UPD") && bcls_rec->Tables.Contains("ROW_CODE"))
		{
			Log::Trace("", "", "修改开始1,count=[{0}]", bcls_rec->Tables["B_UPD"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["B_UPD"].Rows.get_Count(); i++)
			{
				tqmts11a.MergeFrom(bcls_rec->Tables["B_UPD"].Rows[i]);
				tqmts11a.Delete("SEQ_ID,STEEL_GRADE"); //条件字段项

				tqmts11a.MergeFrom(bcls_rec->Tables["B_UPD"].Rows[i]);
				tqmts11a.Insert();

				//修磨要求生成
				doFlag = f_qmts_grind_desc(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					strcpy(s.msg, "调用函数报错!");
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
		}
		if (bcls_rec->Tables.Contains("B_DEL") && bcls_rec->Tables.Contains("ROW_CODE"))
		{
			Log::Trace("", "", "删除开始1,count=[{0}]", bcls_rec->Tables["B_DEL"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["B_DEL"].Rows.get_Count(); i++)
			{
				tqmts11a.MergeFrom(bcls_rec->Tables["B_DEL"].Rows[i]);

				if (tqmts11a["SEQ_ID"].ToString().TrimOrBlank() == " ")
				{
					strcpy(s.msg, _RES("序号不能为空")/*代码不能为空。*/);
					throw CApplicationException(-1, s.msg, log.Location);
				}
				tqmts11a.Delete("SEQ_ID,STEEL_GRADE"); //条件字段项
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
