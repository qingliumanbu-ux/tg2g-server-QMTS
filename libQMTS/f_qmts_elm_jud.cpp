/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2023-08-24 19:54:28
Description: 成分判定函数

**************************************************/

#include "stdafx.h"

BM2_FUNCTION_EXPORT

int f_qmts_elm_cal(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//组合元素计算
int f_qmts_25_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//成分判定
int f_qmts_rep_sel(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//代表成分选定
int f_qmts_stno_set(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//设置出钢记号
int f_qmts_23_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//更新炉次质量信息表
int f_qmbs_sample_select(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//优选代表结果
int f_qmts_elm_jud(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	try
	{
		//复合元素计算
		EIClass elm_cal;
		elm_cal.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
		elm_cal.Tables[0].Columns.Add(DT_STRING, "ST_SAMPLE_NO");
		elm_cal.Tables[0].Columns.Add(DT_STRING, "TABLE_NAME");
		elm_cal.Tables[0].Rows.Add();
		elm_cal.Tables[0].Rows[0]["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString();
		elm_cal.Tables[0].Rows[0]["ST_SAMPLE_NO"] = bcls_rec->Tables[0].Rows[0]["ST_SAMPLE_NO"].ToString();
		elm_cal.Tables[0].Rows[0]["TABLE_NAME"] = "TQMTS25";
		doFlag = f_qmts_elm_cal(&elm_cal, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//判断化学成分是否合格，修改TQMTS25表
		doFlag = f_qmts_25_upd(&elm_cal, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//炉次确定则返回掉
		CModel tpssm11("TPSSM11");//作业计划编制主表
		tpssm11["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString();
		/*if (tpssm11.QueryCount("HEAT_NO") == 0)
		{
		Log::Trace("", "", "炉次确定，不再更新代表成分和物料信息");
		return 0;
		}*/
		//优选代表样
		if ((CString)s.svc_name != "qmbs25s_upd")
		{
			EIClass select_ret;
			doFlag = f_qmbs_sample_select(&elm_cal, &select_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
			Log::Trace("", "", "优选代表样前 ={0}", elm_cal.Tables[0].Rows[0]["ST_SAMPLE_NO"].ToString());

			elm_cal.Tables[0].Rows[0]["ST_SAMPLE_NO"] = select_ret.Tables[0].Rows[0]["ST_SAMPLE_NO"].ToString();

			Log::Trace("", "", "优选代表样后 ={0}", elm_cal.Tables[0].Rows[0]["ST_SAMPLE_NO"].ToString());
		}
		//选择代表成分,新增TQMTS29表
		doFlag = f_qmts_rep_sel(&elm_cal, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//更新TQMTS23表
		EIClass iblk_lh;
		iblk_lh.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
		iblk_lh.Tables[0].Rows.Add();
		iblk_lh.Tables[0].Rows[0]["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString();

		doFlag = f_qmts_23_ins(&iblk_lh, bcls_ret, conn);
		if (doFlag != 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//调用物料函数
		//doFlag = f_qmts_stno_set(&iblk_lh, bcls_ret, conn);
		//if (doFlag != 0)
		//{
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}

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


