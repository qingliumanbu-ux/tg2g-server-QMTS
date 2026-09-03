/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2023-08-30 19:54:28
Description: 代表成分取消
**************************************************/

#include "stdafx.h"

BM2_FUNCTION_EXPORT


int f_qmts_rep_unsel(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CModel tqmts23("TQMTS23");
	CModel tqmts24("TQMTS24");
	CModel tqmts29("TQMTS29");
	CModel tqmtsb0("TQMTSB0");
	CModel tpssm13("TPSSM13");
	CModel tpssm11("TPSSM11");
	CModel tqmts30("TQMTS30");
	CModel hqmts30("HQMTS30");
	try
	{
		tqmts24["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString();
		tqmts24["ST_SAMPLE_NO"] = bcls_rec->Tables[0].Rows[0]["ST_SAMPLE_NO"].ToString();
		Log::Trace("", "", "st_sample_no ={0}", tqmts24["ST_SAMPLE_NO"]);
		Log::Trace("", "", "HEAT_NO ={0}", tqmts24["HEAT_NO"].ToString());
		tqmts24.Query("HEAT_NO,ST_SAMPLE_NO");
		Log::Trace("", "", "REP_ELM_SEL_FLAG ={0}", tqmts24["REP_ELM_SEL_FLAG"].ToString());
		tqmts30["HEAT_NO"] = tqmts24["HEAT_NO"];
		tqmts30["ST_SAMPLE_NO"] = tqmts24["ST_SAMPLE_NO"];
		tqmts30["AREA"] = "北区";
		tqmts30["MAT_NO"] = " ";
		if (tqmts30.Query())
		{
			if (tqmts30["DECIDER"].ToString() != "系统")//手动新增的TQMTS30数据后，就不让连铸结果覆盖了
			{
				return 0;
			}
		}
		if ((CString)s.svc_name == "qmbs25s_upd")//周天成要求，当手动修改成分表时，不清除TQMTS30表
		{
			return 0;
		}
		tqmts30.Delete("HEAT_NO,AREA,MAT_NO");
		if (tqmts24["REP_ELM_SEL_FLAG"].ToString() != "1")
		{
			sprintf(s.msg, "不能取消非代表样");
			Log::Trace("", "", "不能取消非代表样");
			return 0;
		}

		tqmts24["REP_ELM_SEL_FLAG"] = " ";
		tqmts23["REP_ELM_SEL_FLAG"] = " ";
		tqmts23["DECI_ST_NO"] = " ";
		tqmts23["FIN_ST_NO"] = " ";
		tqmts23["JUDGE_CODE"] = " ";
		tqmts23["JUDGE_MAKER"] = " ";
		tqmts23["JUDGE_TIME"] = " ";
		tqmts23["HEAT_NO"] = tqmts24["HEAT_NO"];
		tqmts29["HEAT_NO"] = tqmts24["HEAT_NO"];
		tqmtsb0["HEAT_NO"] = tqmts24["HEAT_NO"];
		tpssm13["HEAT_NO"] = tqmts24["HEAT_NO"];
		tpssm13["REP_ELM_SEL_FLAG"] = " ";
		tpssm11["HEAT_NO"] = tqmts24["HEAT_NO"];
		tpssm11["REP_ELM_SEL_FLAG"] = " ";

		Log::Trace("", "", "line ={0}", __LINE__);
		tqmts24.Update("REP_ELM_SEL_FLAG", "HEAT_NO,ST_SAMPLE_NO");
		tqmts23.Update("REP_ELM_SEL_FLAG,DECI_ST_NO,FIN_ST_NO,JUDGE_CODE,JUDGE_MAKER", "HEAT_NO");
		tqmts29.Delete("HEAT_NO");
		tqmtsb0.Delete("HEAT_NO");
		tpssm13.Update("REP_ELM_SEL_FLAG", "HEAT_NO");
		tpssm11.Update("REP_ELM_SEL_FLAG", "HEAT_NO");

		
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


