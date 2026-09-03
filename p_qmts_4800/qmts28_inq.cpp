/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      wsl
Version:     1.0
Date:        2024.6.6
Description: 同时查询单记录材料信息和委托信息
避免多次查询造成相应速度过慢
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(qmts28_inq)


int f_qmts28_inq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CDbCommand cmd(conn);
	try
	{
		CString q_mat_no = bcls_rec->Tables[0].Rows[0]["MAT_NO"].ToString().Trim();
		CString q_heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
		CString q_slab_cut_time_from = bcls_rec->Tables[0].Rows[0]["SLAB_CUT_TIME_FROM"].ToString().Trim();
		CString q_slab_cut_time_to = bcls_rec->Tables[0].Rows[0]["SLAB_CUT_TIME_TO"].ToString().Trim();
		CString q_rec_create_time_from = bcls_rec->Tables[0].Rows[0]["REC_CREATE_TIME_FROM"].ToString().Trim();
		CString q_rec_create_time_to = bcls_rec->Tables[0].Rows[0]["REC_CREATE_TIME_TO"].ToString().Trim();
			

		Log::Trace("", "", "q_heat_no = {0}", q_heat_no);
		Log::Trace("", "", "q_mat_no = {0}", q_mat_no);
		Log::Trace("", "", "q_slab_cut_time_from = {0}", q_slab_cut_time_from);
		Log::Trace("", "", "q_slab_cut_time_to = {0}", q_slab_cut_time_to);
		Log::Trace("", "", "q_rec_create_time_from = {0}", q_rec_create_time_from);
		Log::Trace("", "", "q_rec_create_time_to = {0}", q_rec_create_time_to);

		sqlstr = "SELECT * FROM  TMMSM01 WHERE 1=1 ";
		if (q_heat_no != "")
		{
			sqlstr += " AND  HEAT_NO = @Heat_no";
		}
		if (q_mat_no != "")
		{
			sqlstr += " AND  MAT_NO = @Mat_no";
		}
		if (q_slab_cut_time_from != "")
		{
			sqlstr += " AND SLAB_CUT_TIME >= @Slab_time_from || '000000' ";
		}
		if (q_slab_cut_time_to != "")
		{
			sqlstr += " AND SLAB_CUT_TIME <= @Slab_time_to ";
		}
		

		Log::Trace("", "", "sqlstr = {0}", sqlstr);
		cmd.SetCommandText(sqlstr);
		cmd.Parameters.Set("Heat_no", q_heat_no);
		cmd.Parameters.Set("Mat_no", q_mat_no);
		cmd.Parameters.Set("Slab_time_from", q_slab_cut_time_from);
		cmd.Parameters.Set("Slab_time_to", q_slab_cut_time_to);
		cmd.ExecuteQuery(bcls_ret->Tables[0]);

		bcls_ret->Tables.Add();
		sqlstr = "SELECT * FROM TQMTS2J WHERE 1=1 ";
		if (q_heat_no != "")
		{
			sqlstr += " AND  HEAT_NO = @Heat_no";
		}
		if (q_rec_create_time_from != "")
		{
			sqlstr += " AND REC_CREATE_TIME >= @rec_time_from || '000000'  ";
		}
		if (q_rec_create_time_to != "")
		{
			sqlstr += " AND REC_CREATE_TIME <= @rec_time_to ";
		}
		cmd.SetCommandText(sqlstr);
		Log::Trace("", "", "sqlstr = {0}", sqlstr);
		cmd.Parameters.Set("Heat_no", q_heat_no);
		cmd.Parameters.Set("rec_time_from", q_rec_create_time_from);
		cmd.Parameters.Set("rec_time_to", q_rec_create_time_to);
		cmd.ExecuteQuery(bcls_ret->Tables[1]);
		Log::Trace("", "", "tqmts2j count = {0}", bcls_ret->Tables[1].Rows.get_Count());
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


