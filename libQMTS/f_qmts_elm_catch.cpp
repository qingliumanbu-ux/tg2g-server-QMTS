/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2023-06-16 19:54:28
Description: 获取成分标准
连铸制造标准直接复制工艺卡数据
其他制造标准，对于存在的元素不复制，对于不存在的元素复制工艺卡数据
**************************************************/

#include "stdafx.h"

BM2_FUNCTION_EXPORT


int f_qmts_elm_catch(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CModel tqmts02("TQMTS02");
	CModel tqmts02_query("TQMTS02");
	CModel hqmts02("HQMTS02");
	CDbCommand cmd(conn);
	try
	{
		tqmts02["WHOLE_BACKLOG_CODE"] = bcls_rec->Tables[1].Rows[0]["WHOLE_BACKLOG_CODE"];
		Log::Trace("", "", "WHOLE_BACKLOG_CODE = {0}", tqmts02["WHOLE_BACKLOG_CODE"]);
		for (size_t i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			tqmts02["ST_NO"] = bcls_rec->Tables[0].Rows[i]["ST_NO"].ToString().Trim();
			tqmts02["FACTORY_DIV"] = bcls_rec->Tables[0].Rows[i]["FACTORY_DIV"].ToString().Trim();
			Log::Trace("", "", "ST_NO = {0}", tqmts02["ST_NO"]);
			Log::Trace("", "", "FACTORY_DIV = {0}", tqmts02["FACTORY_DIV"]);
			tqmts02_query.CopyFrom(tqmts02);
			if (tqmts02["WHOLE_BACKLOG_CODE"].ToString() == "C")//连铸
			{
				return 0;
			}
			sqlstr = "SELECT ELM_CODE, "
				"       ELM_NAME, "
				"       ELM_POS, "
				"       ELM_UNIT, "
				"       MAIN_MIN, "
				"       MAIN_MAX, "
				"       MAIN_AIM, "
				"       SPE_MIN, "
				"       SPE_MAX, "
				"       SMELT_CHEMI_FLAG, "
				"       ROUND_CODE, "
				"       ELM_ACCU "
				"  FROM TQMTS02 "
				" WHERE ST_NO = @st_no "
				"   AND WHOLE_BACKLOG_CODE = 'C' "
				" AND ELM_CODE IN "
				" (SELECT   ELM_CODE  FROM tqmts02 WHERE ST_NO = @st_no AND whole_backlog_code ='C' and factory_div = @factory_div "
				" minus "
				" SELECT   ELM_CODE  FROM tqmts02 WHERE ST_NO = @st_no AND whole_backlog_code = @whole_backlog_code and factory_div = @factory_div) "
				" ORDER BY ELM_POS ASC ";

			cmd.SetCommandText(sqlstr);
			cmd.Parameters.Set("st_no", tqmts02["ST_NO"]);
			cmd.Parameters.Set("factory_div", tqmts02["FACTORY_DIV"]);
			cmd.Parameters.Set("whole_backlog_code", tqmts02["WHOLE_BACKLOG_CODE"]);
			cmd.ExecuteReader();
			while (cmd.Read())
			{
				cmd.Fetch(tqmts02);
				tqmts02["WHOLE_BACKLOG_CODE"] = bcls_rec->Tables[1].Rows[0]["WHOLE_BACKLOG_CODE"];
				tqmts02["ST_NO"] = bcls_rec->Tables[0].Rows[i]["ST_NO"].ToString().Trim();
				tqmts02["FACTORY_DIV"] = bcls_rec->Tables[i].Rows[0]["FACTORY_DIV"].ToString().Trim();
				tqmts02_query["ELM_CODE"] = tqmts02["ELM_CODE"];
				bool exist = tqmts02_query.Query("ST_NO, WHOLE_BACKLOG_CODE, FACTORY_DIV,ELM_CODE");
				if (exist)
				{
					hqmts02.CopyFrom(tqmts02);
					tqmts02["REC_CREATOR"] = s.userid;
					tqmts02["REC_REVISOR"] = s.userid;
					tqmts02["DU_MAKER"] = s.userid;
					tqmts02["REC_CREATE_TIME"] = datetime;
					tqmts02["REC_REVISE_TIME"] = datetime;
					tqmts02["DU_TIME"] = datetime;
					tqmts02["DU_FLAG"] = "U";
					hqmts02["DU_TIME"] = datetime;
					hqmts02["DU_FLAG"] = "U";
					hqmts02.Insert();
					tqmts02["VERSION"] = tqmts02["VERSION"].ToDecimal() + 1;
					tqmts02.Update("*", "ST_NO, WHOLE_BACKLOG_CODE,ELM_CODE");
				}
				else
				{
					tqmts02["REC_CREATOR"] = s.userid;
					tqmts02["REC_REVISOR"] = " ";
					tqmts02["REC_CREATE_TIME"] = datetime;
					tqmts02["REC_REVISE_TIME"] = " ";
					tqmts02["VERSION"] = 0;
					tqmts02.Insert();
				}
			}
			cmd.Close();
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


