/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2023-08-24 19:54:28
Description: 渣样信息子表新增
**************************************************/

#include "stdafx.h"

BM2_FUNCTION_EXPORT


int f_qmts_26_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CModel tqmts26("TQMTS26");//试样信息主表
	CModel tpssm11("TPSSM11");//作业计划编制主表
	CDbCommand cmd(conn);
	try
	{
		//不储存正常返回
		if (bcls_rec->Tables[0].Rows[0]["INSPECTION_TYPE"].ToString() != "S")//不是渣样
		{
			return 0;
		}
		//tqmts26表数据新增
		map<CString, CString> map_code;
		sqlstr = " SELECT CODE_DESC_1_CONTENT,CODE FROM TEP0002 WHERE CODE_CLASS = 'QMZS2N' ";
		cmd.SetCommandText(sqlstr);
		cmd.ExecuteReader();
		while (cmd.Read())
		{
			map_code.insert(pair<CString, CString>(cmd.GetString(1), cmd.GetString(2)));
		}
		cmd.Close();
		tpssm11["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NUMBER"];
		sqlstr = " SELECT * FROM tpssm11 WHERE heat_no = @heat_no"
			"				UNION"
			"				SELECT * FROM tpssm41 WHERE heat_no = @heat_no ";
		CDbCommand cmd_tpssm11(conn);
		cmd_tpssm11.SetCommandText(sqlstr);
		cmd_tpssm11.Parameters.Set("heat_no", tpssm11["HEAT_NO"]);
		cmd_tpssm11.ExecuteReader();
		if (cmd_tpssm11.Read())
		{
			cmd_tpssm11.Fetch(tpssm11);
		}
		cmd_tpssm11.Close();
		tqmts26["ST_SAMPLE_NO"] = bcls_rec->Tables[0].Rows[0]["SAMPLE_ID"];
		tqmts26["REC_CREATOR"] = s.userid;
		tqmts26["REC_CREATE_TIME"] = datetime;
		tqmts26["HEAT_NO"] = tpssm11["HEAT_NO"];
		tqmts26["ST_NO"] = tpssm11["ST_NO"];
		tqmts26["PONO"] = tpssm11["PONO"];

		for (size_t i = 0; i < bcls_rec->Tables[1].Rows.get_Count(); i++)
		{
			tqmts26["ELM_NAME"] = bcls_rec->Tables[1].Rows[i]["ELEMENT"];
			tqmts26["ELM_CODE"] = map_code[tqmts26["ELM_NAME"].ToString()];
			tqmts26["ELM_ACT"] = bcls_rec->Tables[1].Rows[i]["VALUE"];
			//删除tqmts26现有数据
			tqmts26.Delete("HEAT_NO,ST_SAMPLE_NO,ELM_CODE");
			tqmts26.TrimOrBlank();
			tqmts26.Insert();

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


