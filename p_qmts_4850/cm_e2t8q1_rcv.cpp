/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      WSL
Version:     1.0
Date:        2023-11-27 19:08:16
Description: 炉次质量等级值
**************************************************/
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"

BM2F_ENTERACE_TELE(cm_e2t8q1_rcv)

int f_cm_e2t8q1_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CModel tqmts21("TQMTS21");
	CModel tqmts22("TQMTS22");
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	try
	{
		//工位
		tqmts21["STATION_NAME"] = bcls_rec->Tables["INT_MES_QUA_GRADE_VALUE"].Rows[0]["AGGREGATE_NAME"].ToString();
		//熔炼号
		tqmts21["HEAT_NO"] = bcls_rec->Tables["INT_MES_QUA_GRADE_VALUE"].Rows[0]["HEAT_NUMBER"].ToString();
		Log::Trace("", "", "HEAT_NO={0}", tqmts21["HEAT_NO"]);
		//计划号
		tqmts21["PLAN_NO"] = bcls_rec->Tables["INT_MES_QUA_GRADE_VALUE"].Rows[0]["PLAN_NUMBER"].ToString();
		//分包号
		tqmts21["SPLIT_INDICATION"] = bcls_rec->Tables["INT_MES_QUA_GRADE_VALUE"].Rows[0]["SPLIT_INDICATION"].ToString();
		//同工位处理次数
		tqmts21["SAME_PROC_NUM"] = bcls_rec->Tables["INT_MES_QUA_GRADE_VALUE"].Rows[0]["TREATMENT_COUNTER"].ToString();
		//铸机流号
		tqmts21["CAST_MAC_NO"] = bcls_rec->Tables["INT_MES_QUA_GRADE_VALUE"].Rows[0]["STRAND_NUMBER"].ToString();
		//板坯号
		tqmts21["SLAB_NO"] = bcls_rec->Tables["INT_MES_QUA_GRADE_VALUE"].Rows[0]["SLAB_NUMBER"].ToString();
		//生产时刻
		tqmts21["PROD_TIME"] = CDateTime::Parse(bcls_rec->Tables["INT_MES_QUA_GRADE_VALUE"].Rows[0]["GEN_TIME"]).ToString("yyyyMMdd");
		tqmts21["REC_CREATOR"] = s.userid;
		tqmts21["REC_CREATE_TIME"] = datetime;
		tqmts21.TrimOrBlank();
		tqmts21.Delete("HEAT_NO");
		tqmts21.Insert();

		for (int i = 0; i < bcls_rec->Tables["INT_MES_QUA_GRADE_VALUE_DETAIL"].Rows.get_Count(); i++)
		{
			//熔炼号
			tqmts22["HEAT_NO"] = tqmts21["HEAT_NO"].ToString();
			//项标识
			tqmts22["ITEM_POWER"] = bcls_rec->Tables["INT_MES_QUA_GRADE_VALUE_DETAIL"].Rows[i]["ITEM_ID"].ToString();
			//项值
			tqmts22["ITEM_SEQ_NO"] = bcls_rec->Tables["INT_MES_QUA_GRADE_VALUE_DETAIL"].Rows[i]["ITEM_VALUE"].ToString();
			tqmts22.TrimOrBlank();
			tqmts22.Delete("HEAT_NO");
			tqmts22.Insert();
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


