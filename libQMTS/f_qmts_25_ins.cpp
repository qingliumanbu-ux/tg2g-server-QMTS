/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2023-08-24 19:54:28
Description: 试样成分信息子表新增
**************************************************/

#include "stdafx.h"

BM2_FUNCTION_EXPORT


int f_qmts_25_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CModel tqmts25("TQMTS25");//试样信息子表
	CModel tqmts25_old("TQMTS25");//试样信息子表
	CModel hqmts25("HQMTS25");//试样信息子表历史表 
	CModel tqmts23("TQMTS23");//作业计划编制主表
	CDbCommand cmd(conn);
	try
	{
		//不储存正常返回
		if (bcls_rec->Tables[0].Rows[0]["INSPECTION_TYPE"].ToString() == "S")//渣样
		{
			return 0;
		}
		//tqmts25表数据新增
		map<CString, CString> map_code;
		sqlstr = " SELECT CODE,CODE_DESC_1_CONTENT FROM TEP0002 WHERE CODE_CLASS = 'QMYS2N' ";
		cmd.SetCommandText(sqlstr);
		cmd.ExecuteReader();
		while (cmd.Read())
		{
			map_code.insert(pair<CString, CString>(cmd.GetString(2), cmd.GetString(1)));
		}
		cmd.Close();
		tqmts23["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NUMBER"];
		//铁水样查找TPSSM13,TPSSM14表
		if (bcls_rec->Tables[0].Rows[0]["INSPECTION_TYPE"].ToString().Substring(0, 1) == "I")
		{
			sqlstr = " SELECT pono, ST_NO,HEAT_NO  FROM tpssm13 WHERE   heat_no = @heat_no ";
			CDbCommand cmd_tpssm11(conn);
			cmd_tpssm11.SetCommandText(sqlstr);
			cmd_tpssm11.Parameters.Set("heat_no", tqmts23["HEAT_NO"]);
			cmd_tpssm11.ExecuteReader();
			if (cmd_tpssm11.Read())
			{
				cmd_tpssm11.Fetch(tqmts23);
				tqmts25["PONO"] = tqmts23["PONO"];
				tqmts25["ST_NO"] = tqmts23["ST_NO"];
				tqmts25["HEAT_NO"] = tqmts23["HEAT_NO"];
			}
			cmd_tpssm11.Close();
		}
		else
		{
			if (!tqmts23.Query("HEAT_NO"))
			{
				//钢水样查找制造命令和出钢记号
				sqlstr = " SELECT * FROM tpssm11 WHERE heat_no = @heat_no"
					"				UNION"
					"				SELECT * FROM tpssm41 WHERE heat_no = @heat_no ";
				CDbCommand cmd_tpssm11(conn);
				cmd_tpssm11.SetCommandText(sqlstr);
				cmd_tpssm11.Parameters.Set("heat_no", tqmts23["HEAT_NO"]);
				cmd_tpssm11.ExecuteReader();
				if (cmd_tpssm11.Read())
				{
					cmd_tpssm11.Fetch(tqmts23);
				}
				cmd_tpssm11.Close();
			}
			tqmts25["PONO"] = tqmts23["PONO"];
			tqmts25["ST_NO"] = tqmts23["ST_NO"];
			tqmts25["HEAT_NO"] = tqmts23["HEAT_NO"];
		}
		tqmts25["ST_SAMPLE_NO"] = bcls_rec->Tables[0].Rows[0]["SAMPLE_ID"];
		tqmts25["REC_CREATOR"] = s.userid;
		tqmts25["REC_CREATE_TIME"] = datetime;
		

		for (size_t i = 0; i < bcls_rec->Tables[1].Rows.get_Count(); i++)
		{
			if (bcls_rec->Tables[1].Rows[i]["ELEMENT"].ToString() == "Fe%" || bcls_rec->Tables[1].Rows[i]["ELEMENT"].ToString() == "Alsol" || bcls_rec->Tables[1].Rows[i]["ELEMENT"].ToString() == "Aloxy" || bcls_rec->Tables[1].Rows[i]["ELEMENT"].ToString() == "AlsolE" || bcls_rec->Tables[1].Rows[i]["ELEMENT"].ToString() == "AloxyE" || bcls_rec->Tables[1].Rows[i]["ELEMENT"].ToString() == "F" || bcls_rec->Tables[1].Rows[i]["ELEMENT"].ToString() == "E" || bcls_rec->Tables[1].Rows[i]["ELEMENT"].ToString() == "Alinsol")
			{
				Log::Trace("", "", "[{0}]元素跳过", bcls_rec->Tables[1].Rows[i]["ELEMENT"].ToString());
				continue; 
			}
			tqmts25["ELM_NAME"] = bcls_rec->Tables[1].Rows[i]["ELEMENT"];
			tqmts25["ELM_CODE"] = map_code[tqmts25["ELM_NAME"].ToString()];
			tqmts25["ELM_ACT"] = bcls_rec->Tables[1].Rows[i]["VALUE"];
			tqmts25["ELM_ACT_OLD"] = bcls_rec->Tables[1].Rows[i]["VALUE"];
			//保存历史记录
			tqmts25_old.CopyFrom(tqmts25);
			if (tqmts25_old.Query("HEAT_NO,ST_SAMPLE_NO,ELM_CODE"))
			{
				if (tqmts25_old["ELM_ACT_OLD"].ToDecimal() != tqmts25["ELM_ACT_OLD"].ToDecimal())
				{
					hqmts25.CopyFrom(tqmts25_old);
					hqmts25["ELM_ACT"] = tqmts25["ELM_ACT"];
					hqmts25["REC_REVISOR"] = s.userid;
					hqmts25["REC_REVISE_TIME"] = datetime;
					hqmts25["COMPANY_NAME"] = s.svc_name;
					hqmts25.TrimOrBlank();
					hqmts25.Insert();
				}
			}
			//删除tqmts25现有数据
			tqmts25.Delete("HEAT_NO,ST_SAMPLE_NO,ELM_CODE");
			tqmts25.Insert();
			if (bcls_rec->Tables[1].Rows[i]["ELEMENT"].ToString() == "Al")
			{
				tqmts25["ELM_NAME"] = "Alt";
				tqmts25["ELM_CODE"] = "035";
				tqmts25.Delete("HEAT_NO,ST_SAMPLE_NO,ELM_CODE");
				tqmts25.Insert();
			}
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


