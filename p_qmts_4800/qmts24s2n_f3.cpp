/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2021-04-28 15:08:16
Description: service模板
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(qmts24s2n_f3)
int f_qmts_elm_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//成分接收函数
int f_qmts24s2n_f3(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	try
	{
		CModel tqmts24_init("TQMTS24_INIT");
		tqmts24_init.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		if (tqmts24_init["WHOLE_BACKLOG_CODE"].ToString() != "C")
		{
			sprintf(s.msg, "不是连铸结果，不可以重新接收");
			throw CApplicationException(-1, s.msg, log.Location);
		}
		Log::Trace("", "", "elm_id ={0}", tqmts24_init["ID_ELM"].ToString());
		EIClass elm_rcv;
		elm_rcv.Tables[0].Columns.Add(DT_STRING, "AGGREGATE_NAME");
		elm_rcv.Tables[0].Columns.Add(DT_STRING, "analysis_time");
		elm_rcv.Tables[0].Columns.Add(DT_STRING, "bad_sample");
		elm_rcv.Tables[0].Columns.Add(DT_STRING, "elementcount");
		elm_rcv.Tables[0].Columns.Add(DT_STRING, "grade");
		elm_rcv.Tables[0].Columns.Add(DT_STRING, "heat_number");
		elm_rcv.Tables[0].Columns.Add(DT_STRING, "id");
		elm_rcv.Tables[0].Columns.Add(DT_STRING, "inspection_type");
		elm_rcv.Tables[0].Columns.Add(DT_STRING, "plan_id");
		elm_rcv.Tables[0].Columns.Add(DT_STRING, "sample_id");
		elm_rcv.Tables[0].Columns.Add(DT_STRING, "sample_number");
		elm_rcv.Tables[0].Columns.Add(DT_STRING, "sample_source");
		elm_rcv.Tables[0].Columns.Add(DT_STRING, "sample_taken_time");
		elm_rcv.Tables[0].Columns.Add(DT_STRING, "sample_type");
		elm_rcv.Tables[0].Rows.Add();
		elm_rcv.Tables.Add();
		elm_rcv.Tables[1].Columns.Add(DT_STRING, "element");
		elm_rcv.Tables[1].Columns.Add(DT_STRING, "id");
		elm_rcv.Tables[1].Columns.Add(DT_STRING, "sample_id");
		elm_rcv.Tables[1].Columns.Add(DT_STRING, "value");
		//赋值
		elm_rcv.Tables[0].Rows[0]["PLAN_ID"] = tqmts24_init["SM_PLAN_NO"];
		elm_rcv.Tables[0].Rows[0]["INSPECTION_TYPE"] = tqmts24_init["ST_SAMPLE_DIV"];//T 钢水样 G气体样 I铁样 P铸坯样
		elm_rcv.Tables[0].Rows[0]["AGGREGATE_NAME"] = tqmts24_init["DEV_CODE"];//工位
		elm_rcv.Tables[0].Rows[0]["HEAT_NUMBER"] = tqmts24_init["HEAT_NO"];
		elm_rcv.Tables[0].Rows[0]["SAMPLE_TAKEN_TIME"] = tqmts24_init["SAMPLE_TAKEN_TIME"];
		elm_rcv.Tables[0].Rows[0]["SAMPLE_NUMBER"] = tqmts24_init["ST_SAMPLE_SEQ"];
		elm_rcv.Tables[0].Rows[0]["AGGREGATE_NAME"] = tqmts24_init["WHOLE_BACKLOG_CODE"];
		elm_rcv.Tables[0].Rows[0]["SAMPLE_TYPE"] = tqmts24_init["SAMPLE_TYPE"];
		elm_rcv.Tables[0].Rows[0]["GRADE"] = tqmts24_init["ST_NO"];
		elm_rcv.Tables[0].Rows[0]["BAD_SAMPLE"] = tqmts24_init["SAMPLE_IFGOOD"];
		elm_rcv.Tables[0].Rows[0]["ANALYSIS_TIME"] = tqmts24_init["ANALYSE_TIME"];//分析时间
		elm_rcv.Tables[0].Rows[0]["SAMPLE_TAKEN_TIME"] = tqmts24_init["SAMPLE_TAKEN_TIME"];
		elm_rcv.Tables[0].Rows[0]["ELEMENTCOUNT"] = tqmts24_init["ELEMENTCOUNT"];//元素个数
		
		elm_rcv.Tables[0].Rows[0]["ID"] = tqmts24_init["ID_ELM"];
		elm_rcv.Tables[0].Rows[0]["SAMPLE_ID"] = tqmts24_init["ST_SAMPLE_NO"];
		
		map<CString, CString> map_code;
		CDbCommand cmd(conn);
		sqlstr = " SELECT CODE_DESC_1_CONTENT,CODE FROM TEP0002 WHERE CODE_CLASS = 'QMYS2N' ";
		cmd.SetCommandText(sqlstr);
		cmd.ExecuteReader();
		while (cmd.Read())
		{
			map_code.insert(pair<CString, CString>(cmd.GetString(2), cmd.GetString(1)));
		}
		for (size_t i = 0; i < bcls_rec->Tables[0].Columns.get_Count(); i++)
		{
			
			if (bcls_rec->Tables[0].Columns[i].get_ColumnName().GetLength() == 7)
			{
				if (bcls_rec->Tables[0].Columns[i].get_ColumnName().Substring(0, 4) == "ELM_")
				{
					if (bcls_rec->Tables[0].Rows[0][i].ToDecimal() != -1)
					{
						Log::Trace("", "", "bcls_rec->Tables[0].Columns[i].get_ColumnName() = {0} ", bcls_rec->Tables[0].Columns[i].get_ColumnName().Substring(4, 3));
						elm_rcv.Tables[1].Rows.Add();
						int count = elm_rcv.Tables[1].Rows.get_Count() - 1;
						elm_rcv.Tables[1].Rows[count]["element"] = map_code[bcls_rec->Tables[0].Columns[i].get_ColumnName().Substring(4, 3)];
						elm_rcv.Tables[1].Rows[count]["id"] = tqmts24_init["ID_ELM"];
						elm_rcv.Tables[1].Rows[count]["sample_id"] = tqmts24_init["ST_SAMPLE_NO"];
						elm_rcv.Tables[1].Rows[count]["value"] = bcls_rec->Tables[0].Rows[0][i];
					}
				}
			}
		}
		doFlag = f_qmts_elm_rcv(&elm_rcv, bcls_ret, conn);
		if (doFlag != 0)
		{
			Log::Trace("", "", "f_qmts_elm_rcv() msg = [{0}]", s.msg);
			throw CApplicationException(-1, s.msg, log.Location);
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
