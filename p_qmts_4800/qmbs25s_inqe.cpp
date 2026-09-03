/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2023-10-01 14:48:51
Description: 后备录入查询
**************************************************/

#include "stdafx.h"
BM2F_ENTERACE(qmbs25s_inqe)

int f_qmbs25s_inqe(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CModel tpssm11("TPSSM11");

	try
	{
		/* 获取传入的表名 */
		CDbCommand cmd(conn);
		CString heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"];
		CString st_sample_no = bcls_rec->Tables[0].Rows[0]["ST_SAMPLE_NO"];
		sqlstr = " SELECT PONO, ST_NO,HEAT_NO  FROM TPSSM11 WHERE HEAT_NO = @heat_no "
			"			UNION"
			"			SELECT PONO, ST_NO,HEAT_NO  FROM TPSSM41 WHERE HEAT_NO = @heat_no ";
		cmd.SetCommandText(sqlstr);
		cmd.Parameters.Set("heat_no", heat_no);
		cmd.ExecuteReader();
		if (cmd.Read())
		{
			cmd.Fetch(tpssm11);
		}
		cmd.Close();
		sqlstr = " select ST_NO from tqmts24 where ST_SAMPLE_NO='"+ st_sample_no +"' ";
		cmd.SetCommandText(sqlstr);
		cmd.Parameters.Set("heat_no", heat_no);
		cmd.ExecuteReader();
		if (cmd.Read())
		{
			if (tpssm11["ST_NO"].ToString()!=cmd.GetString(1))
			{
				tpssm11["ST_NO"] = cmd.GetString(1);
			}
		}
		cmd.Close();
		
		sqlstr = " SELECT TQMTS02.ELM_CODE, CODE_DESC_3_CONTENT AS  ELM_NAME, CODE_DESC_1_CONTENT AS  ELM_ENAME,MAIN_MIN, MAIN_MAX, MAIN_AIM,SPE_MIN,SPE_MAX,T2.ELM_ACT,T2.ELM_OK  FROM TQMTS0X LEFT JOIN TQMTS02"
			"			ON TQMTS0X.ELM_STD_IDX_A = TQMTS02.IDX_NO"
			"			LEFT JOIN(SELECT * FROM TEP0002 WHERE CODE_CLASS = 'QMYS2N') T1 ON TQMTS02.ELM_CODE = T1.CODE"
			"			LEFT JOIN(SELECT * FROM TQMTS25 WHERE ST_SAMPLE_NO = @st_sample_no) T2 ON TQMTS02.ELM_CODE = T2.ELM_CODE"
			"			WHERE TQMTS0X.ST_NO = @st_no "
			" UNION " //查询标准内没有的元素
			" SELECT"
			" ELM_CODE,"
			" CODE_DESC_3_CONTENT AS ELM_NAME,"
			" ELM_NAME AS ELM_ENAME,"
			" 0 AS MAIN_MIN,"
			" 0 AS MAIN_MAX,"
			" 0 AS MAIN_AIM,"
			" 0 AS SPE_MIN,"
			" 0 AS SPE_MAX,"
			" ELM_ACT,"
			" ELM_OK"
			" FROM"
			" TQMTS25"
			" LEFT JOIN("
			" SELECT"
			" *"
			" FROM"
			" TEP0002"
			" WHERE"
			" CODE_CLASS = 'QMYS2N') T2 ON"
			" T2.CODE = TQMTS25.ELM_CODE"
			" WHERE"
			" ST_SAMPLE_NO = @st_sample_no"
			" AND ELM_CODE IN("
			" SELECT"
			" CODE"
			" FROM"
			" tep0002"
			" WHERE"
			" CODE_CLASS = 'QMYS2N'"
			" AND CODE IN('001',"//如果还缺，就这里加上元素代码
			"'002',"
			"'003',"
			"'004',"
			"'005',"
			"'006',"
			"'007',"
			"'008',"
			"'009',"
			"'010',"
			"'011',"
			"'012',"
			"'013',"
			"'014',"
			"'015',"
			"'016',"
			"'017',"
			"'018',"
			"'019',"
			"'020',"
			"'021',"
			"'022',"
			"'023',"
			"'024',"
			"'025',"
			"'026',"
			"'029',"
			"'030',"
			"'031',"
			"'033',"
			"'036') MINUS"
			" SELECT"
			" TQMTS02.ELM_CODE"
			" FROM"
			" TQMTS0X"
			" LEFT JOIN TQMTS02"
			" ON"
			" TQMTS0X.ELM_STD_IDX_A = TQMTS02.IDX_NO"
			" LEFT JOIN("
			" SELECT"
			" *"
			" FROM"
			" TEP0002"
			" WHERE"
			" CODE_CLASS = 'QMYS2N') T1 ON"
			" TQMTS02.ELM_CODE = T1.CODE"
			" LEFT JOIN("
			" SELECT"
			" *"
			" FROM"
			" TQMTS25"
			" WHERE"
			" ST_SAMPLE_NO = @st_sample_no) T2 ON"
			" TQMTS02.ELM_CODE = T2.ELM_CODE"
			" WHERE"
			" TQMTS0X.ST_NO = @st_no) ";
		Log::Trace("", __FUNCTION__, " sqlstr [{0}]  ", sqlstr);
		cmd.SetCommandText(sqlstr);
		cmd.Parameters.Set("st_sample_no", st_sample_no);
		cmd.Parameters.Set("st_no", tpssm11["ST_NO"]);
		int count = cmd.ExecuteQuery(bcls_ret->Tables[0]);
		bcls_ret->Tables[0].set_TableName("TQMTS25");
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
