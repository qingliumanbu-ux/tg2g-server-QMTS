/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2023-06-13 19:54:28
Description: 更新炉次质量表TQMTS23
**************************************************/

#include "stdafx.h"


BM2_FUNCTION_EXPORT

int f_qmts_23_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	int ret;
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CModel tqmts23("TQMTS23");//炉次质量信息表
	CModel tpssm11("TPSSM11");//作业计划编制主表
	CModel tqmtsb0("TQMTSB0");//代表成分表
	CDbCommand cmd(conn);
	try
	{
		tqmts23["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO"];
		Log::Trace("", "", "heat_no = {0}", tqmts23["HEAT_NO"].ToString());
		/*tqmts23表初始化赋值*/
		if (!tqmts23.Query("HEAT_NO"))
		{
			tpssm11["HEAT_NO"] = tqmts23["HEAT_NO"];
			sqlstr = " SELECT * FROM tpssm11 WHERE heat_no = @heat_no"
				"				UNION"
				"				SELECT * FROM tpssm41 WHERE heat_no = @heat_no ";
			CDbCommand cmd_tpssm11(conn);
			cmd_tpssm11.SetCommandText(sqlstr);
			cmd_tpssm11.Parameters.Set("heat_no", tpssm11["HEAT_NO"]);
			cmd_tpssm11.ExecuteReader();
			if (cmd_tpssm11.Read()){
				cmd_tpssm11.Fetch(tpssm11);
			}
			cmd_tpssm11.Close();
			/*tqmts23表初始化赋值*/
			tqmts23.Query("HEAT_NO");
			if (tqmts23["JUDGE_ST_NO"].ToString().Trim() != "")
			{
				Log::Trace("", "", "炉次已经有判定出钢记号");
				return 0;
			}
			tqmts23["PONO"] = tpssm11["PONO"];
			tqmts23["ST_NO"] = tpssm11["ST_NO"];
			tqmts23["REC_CREATOR"] = s.userid;
			tqmts23["REC_CREATE_TIME"] = datetime;
			tqmts23["COMPANY_NAME"] = s.svc_name;
		}
		tqmts23["REC_REVISOR"] = s.userid;
		tqmts23["REC_REVISE_TIME"] = datetime;
		/**查询代表试样是否是不合格**/
		sqlstr = "SELECT * FROM TQMTS24 WHERE   HEAT_NO = @heat_no  AND REP_ELM_SEL_FLAG = '1' AND JUDGE_CODE = '2'";
		cmd.SetCommandText(sqlstr);
		cmd.Parameters.Set("heat_no", tqmts23["HEAT_NO"]);
		cmd.ExecuteReader();
		int fechCount = 0;
		tqmts23["JUDGE_REMARK"] = " ";
		tqmts23["YY_CAUSE"] = "";
		tqmts23["REP_ELM_SEL_FLAG"] = "1";
		if (cmd.Read()){
			fechCount++;
			Log::Trace("", "", "试样不合格");
			tqmts23["JUDGE_CODE"] = "2";
			if (fechCount == 1){
				tqmts23["YY_CAUSE"] = "10";
			}
			tqmts23["FIN_ST_NO"] = " ";
			tqmts23["DECI_ST_NO"] = "YY000000";
		}
		if (!fechCount){
			Log::Trace("", "", "无炉次异常情况");
			Log::Trace("", "", "tqmts23.YY_CAUSE = {0}", tqmts23["YY_CAUSE"].ToString());
			Log::Trace("", "", "tqmts23.FIN_ST_NO = {0}", tqmts23["FIN_ST_NO"].ToString());

			tqmtsb0["HEAT_NO"] = tqmts23["HEAT_NO"];
			tqmts23["DECI_ST_NO"] = tqmts23["ST_NO"];
			tqmts23["FIN_ST_NO"] = tqmts23["ST_NO"];
			tqmts23["JUDGE_CODE"] = "1";
			tqmts23["JUDGE_MAKER"] = s.userid;
			tqmts23["JUDGE_TIME"] = datetime;
			tqmts23["YY_CAUSE"] = "99";
			/*没有代表成分*/
			if (tqmtsb0.QueryCount("HEAT_NO") == 0)
			{
				Log::Trace("", "", "没有代表成分");
				tqmts23["DECI_ST_NO"] = " ";
				tqmts23["FIN_ST_NO"] = " ";
				tqmts23["JUDGE_CODE"] = " ";
				tqmts23["REP_ELM_SEL_FLAG"] = " ";
				tqmts23["JUDGE_MAKER"] = " ";
				tqmts23["JUDGE_TIME"] = " ";
				tqmts23["YY_CAUSE"] = " ";
			}
		}
		cmd.Close();
		//更新TQMTS23表
		tqmts23.Delete("HEAT_NO");
		tqmts23.TrimOrBlank();
		tqmts23.Insert();
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


