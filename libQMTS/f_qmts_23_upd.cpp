/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2024-04-23 18:54:28
Description: 根据信号修改连铸代表成分对应的出钢记号，
			 1、重新判定连铸成分
			 2、更新TQMTS23表的判定出钢记号、计划号

开浇点调用：新增TQMTS23表，PONO,SM_PLAN_NO,HEAT_NO,ST_NO
二级上信号炉号和制造命令对应关系时：修改TQMTS23表 PONO,SM_PLAN_NO,HEAT_NO,ST_NO，重新进行成分判定和发送四级代表成分

业务情况：
1.检化验上传成分时，TPSSM11表对应关系和二级不一致。此时如果成分合格上传给四级，三四级TQMTQQ0表内的熔炼号和
制造命令会对应错误，此时应当修正TQMTS23表的制造命令和熔炼号的对应关系
2.TPSSM11表内可能存在一个熔炼号对应多个计划的情况，查询时可能存在错误数据，成分接收时应优先查询TQMTS23表，如无数据，则查询TPSSM11表

调用方式:
bcls_rec->Tables[0].Columns.Add(DT_STRING, "SM_PLAN_NO");  --计划号
bcls_rec->Tables[0].Columns.Add(DT_STRING, "HEAT_NO");      --熔炼号
bcls_rec->Tables[0].Columns.Add(DT_STRING, "PONO");      --制造命令
bcls_rec->Tables[0].Columns.Add(DT_STRING, "ST_NO");	--出钢记号
bcls_rec->Tables[0].Rows.Add();
bcls_rec->Tables[0].Rows[0]["SM_PLAN_NO"] = "5460";
bcls_rec->Tables[0].Rows[0]["HEAT_NO"] = "A2300002";
bcls_rec->Tables[0].Rows[0]["ST_NO"] = "1A3041";
doFlag = f_qmts_23_upd(bcls_rec, bcls_ret, conn);
if (doFlag != 0)
{
throw CApplicationException(-1, s.msg, log.Location);
}
**************************************************/

#include "stdafx.h"

BM2_FUNCTION_EXPORT

int f_qmts_25_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//成分判定
int f_qmbs_sample_select(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);//试样选择
int f_qmts_rep_sel(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_qmts_stno_set(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

int f_qmts_23_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CModel tqmts23("TQMTS23");
	CModel tqmts24("TQMTS24");
	CModel tqmts25("TQMTS25");
	try
	{
		Log::Trace("", "", "heat_no = {0}", bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString());
		Log::Trace("", "", "st_no = {0}", bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString());
		Log::Trace("", "", "sm_plan_no = {0}", bcls_rec->Tables[0].Rows[0]["SM_PLAN_NO"].ToString());
		Log::Trace("", "", "PONO = {0}", bcls_rec->Tables[0].Rows[0]["PONO"].ToString());
		//更新炉次的出钢记号、计划号等信息
		tqmts23["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO"];
		if (!tqmts23.Query())
		{
			Log::Trace("", "", "未查到炉次信息");
			return 0;
		}
		tqmts23["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString();
		if (tqmts23["ST_NO"].ToString() != bcls_rec->Tables[0].Rows[0]["ST_NO"].ToString())//改钢种
		{
			tqmts23["JUDGE_MAKER"] = s.userid;
			tqmts23["JUDGE_TIME"] = datetime;
			tqmts23["JUDGE_ST_NO"] = bcls_rec->Tables[0].Rows[0]["ST_NO"];
		}
		tqmts23["ST_NO"] = bcls_rec->Tables[0].Rows[0]["ST_NO"];
		tqmts23["FIN_ST_NO"] = bcls_rec->Tables[0].Rows[0]["ST_NO"];
		tqmts23["SM_PLAN_NO"] = bcls_rec->Tables[0].Rows[0]["SM_PLAN_NO"];
		tqmts23["PONO"] = bcls_rec->Tables[0].Rows[0]["PONO"];
		tqmts23["COMPANY_NAME"] = s.svc_name;
		tqmts23.Update("*");

		//重新判定连铸成分
		//1 更新TQMTS24试样信息表
		tqmts24["HEAT_NO"] = tqmts23["HEAT_NO"];
		tqmts24["ST_NO"] = tqmts23["ST_NO"];
		tqmts24["WHOLE_BACKLOG_CODE"] = "C";
		tqmts24["PONO"] = bcls_rec->Tables[0].Rows[0]["PONO"];
		tqmts24["REC_REVISE_TIME"] = datetime;
		tqmts24["REC_REVISOR"] = s.svc_name;
		tqmts24.Update("ST_NO,REC_REVISE_TIME,REC_REVISOR,PONO", "HEAT_NO");

		//更新TQMTS25表试样信息
		tqmts25["HEAT_NO"] = tqmts23["HEAT_NO"];
		tqmts25["ST_NO"] = tqmts23["ST_NO"];
		tqmts25["WHOLE_BACKLOG_CODE"] = "C";
		tqmts25["PONO"] = bcls_rec->Tables[0].Rows[0]["PONO"];
		tqmts25["REC_REVISE_TIME"] = datetime;
		tqmts25["REC_REVISOR"] = s.svc_name;
		tqmts25.Update("ST_NO,REC_REVISE_TIME,REC_REVISOR,PONO", "HEAT_NO");

		//2 重新判定TQMTS25试样信息子表
		sqlstr = "SELECT ST_SAMPLE_NO  FROM tqmts24 WHERE HEAT_NO = @heat_no AND WHOLE_BACKLOG_CODE = 'C'";
		CDbCommand cmd(conn);
		cmd.SetCommandText(sqlstr);
		cmd.Parameters.Set("heat_no", tqmts23["HEAT_NO"].ToString());
		cmd.ExecuteReader();
		EIClass elm_cal;
		elm_cal.Tables[0].Columns.Add(DT_STRING, "HEAT_NO");
		elm_cal.Tables[0].Columns.Add(DT_STRING, "ST_SAMPLE_NO");
		elm_cal.Tables[0].Columns.Add(DT_STRING, "TABLE_NAME");
		elm_cal.Tables[0].Rows.Add();
		while (cmd.Read())
		{
			elm_cal.Tables[0].Rows[0]["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString();
			elm_cal.Tables[0].Rows[0]["ST_SAMPLE_NO"] = cmd.GetString(1);
			elm_cal.Tables[0].Rows[0]["TABLE_NAME"] = "TQMTS25";
			//判断化学成分是否合格，修改TQMTS25表
			doFlag = f_qmts_25_upd(&elm_cal, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		
		//3 重新选择代表成分
		EIClass select_ret;
		doFlag = f_qmbs_sample_select(&elm_cal, &select_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}

		elm_cal.Tables[0].Rows[0]["ST_SAMPLE_NO"] = select_ret.Tables[0].Rows[0]["ST_SAMPLE_NO"].ToString();

		doFlag = f_qmts_rep_sel(&elm_cal, bcls_ret, conn);
		if (doFlag < 0)
		{
			doFlag = 0;
			s.flag = 0;
			sprintf(s.msg, "");
			//throw CApplicationException(-1, s.msg, log.Location);
		}

		//调用物料函数
		//doFlag = f_qmts_stno_set(&elm_cal, bcls_ret, conn);
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


