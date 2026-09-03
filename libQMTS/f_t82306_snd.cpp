/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2024-1-03 9:13:28
Description: 二炼钢炉次投料接收电文
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_t82306_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmm");
	EPEX epex;
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn);
	CDbCommand cmd_sql11(conn);
	CDbCommand cmd_sqla1(conn);
	CDbCommand cmd_sqlsj(conn);
	CModel tmmsm2a("TMMSM2A");
	CModel tmmsm27("TMMSM27");
	CModel tmmsm24("TMMSM24");
	CModel tmmsm26("TMMSM26");
	CModel tmmsm31("TMMSM31");
	CModel tmmsm01("TMMSM01");
	CModel tpssm11("TPSSM11");
	CModel hmmsm2a("HMMSM2A");
	CString consign_wt = "";//收货重量
	CString t_accepttime = "";//收货时间
	try
	{
		CString epex_number = "T82306";
		CString deal_flag = bcls_rec->Tables["T823"].Rows[0]["DEAL_FLAG"].ToString().Trim();
		if (epex.Initialize(epex_number) < 0)
		{
			sprintf(s.msg, (const char*)epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location); 
		}
		tmmsm2a.MergeFrom(bcls_rec->Tables["T823"].Rows[0]);
		hmmsm2a.MergeFrom(bcls_rec->Tables["T823"].Rows[0]);
		//操作标记
		if (epex.SetValue("DEAL_FLAG", 0, deal_flag) < 0)
		{
			sprintf(s.msg, (const char*)epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//投料工序
		if (epex.SetValue("WHOLE_BACKLOG_CODE", 0, tmmsm2a["DEV_CODE"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		Log::Info("", __FUNCTION__, "WHOLE_BACKLOG_CODE   =[{0}]", tmmsm2a["DEV_CODE"].ToString());
		//炼钢记号
		if (epex.SetValue("ST_NO", 0, tmmsm2a["ST_NO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}Log::Info("", __FUNCTION__, "ST_NO_1   =[{0}]", tmmsm2a["ST_NO"].ToString());

		//炉号
		if (epex.SetValue("HEATNO", 0, tmmsm2a["HEAT_NO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//AOD_LEAVE_WT AOD出钢重量
		CString end_time = " ";
		CDecimal aod_leave_wt = 0;
		cmd_sqla1.SetCommandText("  SELECT END_TIME,ACTRESULT, SM_PLAN_NOL2 FROM("
			" select END_TIME, ACTRESULT,SM_PLAN_NOL2 from TMMSM27 "
			" UNION "
			" SELECT END_TIME,ACTRESULT, SM_PLAN_NOL2 FROM TMMSM21 "
			" ) WHERE SM_PLAN_NOL2 = '" + tmmsm2a["SM_PLAN_NOL2"].ToString() + "' ");
		cmd_sqla1.ExecuteReader();
		if (cmd_sqla1.Read())
		{
			end_time = cmd_sqla1.GetString(1);
			aod_leave_wt = cmd_sqla1.GetDecimal(2);
		}
		Log::Trace("", "end_time", "end_time = {0}", end_time);
		cmd_sqla1.Close();
		hmmsm2a["ACTRESULT"] = aod_leave_wt;
		hmmsm2a["END_TIME"] = end_time;
		if (epex.SetValue("AOD_LEAVE_WT", 0, aod_leave_wt) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//LF_LEAVE_WT LF出站重量//刚水量
		tmmsm24["SM_PLAN_NOL2"] = tmmsm2a["SM_PLAN_NOL2"].ToString().Trim();
		tmmsm24["HEAT_NO"] = tmmsm2a["HEAT_NO"].ToString();
		tmmsm24.Query("SM_PLAN_NOL2,HEAT_NO");
		tmmsm24.TrimOrBlank();
		hmmsm2a["MOLTIRON_WT"] = tmmsm24["MOLTIRON_WT"].ToString();
		if (epex.SetValue("LF_LEAVE_WT", 0, tmmsm24["MOLTIRON_WT"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//LTS_LEAVE_WT LTS出站重量 STEEL_WEIGHT
		tmmsm26["SM_PLAN_NOL2"] = tmmsm2a["SM_PLAN_NOL2"].ToString().Trim();
		tmmsm26["HEAT_NO"] = tmmsm2a["HEAT_NO"].ToString();
		tmmsm26.Query("SM_PLAN_NOL2,HEAT_NO");
		tmmsm26.TrimOrBlank();
		hmmsm2a["STEEL_WT"] = tmmsm26["STEEL_WT"].ToString();
		if (epex.SetValue("LTS_LEAVE_WT", 0, tmmsm26["STEEL_WT"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//CC_LEAVE_WT 连铸接收重量  LADLE_ARRIVE_WT -LADLE_LEAVE_WT
		tmmsm31["SM_PLAN_NOL2"] = tmmsm2a["SM_PLAN_NOL2"].ToString().Trim();
		tmmsm31["HEAT_NO"] = tmmsm2a["HEAT_NO"].ToString();
		tmmsm31.Query("SM_PLAN_NOL2,HEAT_NO");
		tmmsm31.TrimOrBlank();
		CDecimal cc_leave_wt=tmmsm31["LADLE_ARRIVE_WT"].ToDecimal() - tmmsm31["LADLE_LEAVE_WT"].ToDecimal();
		hmmsm2a["CC_LEAVE_WT"] = cc_leave_wt;
		if (epex.SetValue("CC_LEAVE_WT", 0, cc_leave_wt) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//根据熔炼号查询01表
		tmmsm01["SM_PLAN_NOL2"] = tmmsm2a["SM_PLAN_NOL2"].ToString().Trim();
		cmd_inq.SetCommandText(" SELECT  MAX(RECV_MAT_TIME),SUM(RECEIVE_WEIGHT) FROM ( "
			" SELECT MAX(RECV_MAT_TIME) RECV_MAT_TIME, SUM(NVL(RECEIVE_WEIGHT, 0)) RECEIVE_WEIGHT FROM TMMSM01 WHERE CONCESS_CON_FLAG!='1'  and mat_no not in (select in_mat_no  from tmmsm35) and SM_PLAN_NO = '" + tmmsm31["SM_PLAN_NOL2"].ToString() + "' "
			" UNION "
			" SELECT MAX(RECV_MAT_TIME), SUM(NVL(RECEIVE_WEIGHT, 0)) RECEIVE_WEIGHT FROM HMMSM01 WHERE CONCESS_CON_FLAG!='1'  and mat_no not in (select in_mat_no  from tmmsm35) and SM_PLAN_NO = '" + tmmsm31["SM_PLAN_NOL2"].ToString() + "' "
			" ) ");
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			t_accepttime = cmd_inq.GetString(1);
			consign_wt = cmd_inq.GetString(2);
		}
		if (consign_wt == "")
		{
			consign_wt = "0";
		}
		if (t_accepttime == "")
		{
			t_accepttime = " ";
		}
		Log::Trace("", "consign_wt,t_accepttime", "consign_wt = {0},t_accepttime={1}", consign_wt, t_accepttime);
		cmd_inq.Close();
		//CONSIGN_WT 收货重量
		hmmsm2a["RECEIVE_WEIGHT"] = consign_wt;
		hmmsm2a["RECV_MAT_TIME"] = t_accepttime;
		hmmsm2a["END_TIME"] = t_accepttime;
		if (epex.SetValue("CONSIGN_WT", 0, consign_wt) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//END_TIME 结束时间
		if (epex.SetValue("END_TIME", 0, end_time) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//收货时间
		if (epex.SetValue("T_ACCEPTTIME", 0, t_accepttime) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//物料号
		if (epex.SetValue("MATNR", 0, tmmsm2a["MAT_CODE"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//物料描述
		if (epex.SetValue("MAKTX", 0, tmmsm2a["MAT_NAME"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//投料重量
		if (epex.SetValue("DEVO_WT", 0, tmmsm2a["DEVO_WT"].ToDecimal()/1000) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		hmmsm2a["REC_CREATE_TIME"] = datetime+"00";
		hmmsm2a.Insert();
		if (epex.SendTele() < 0)
		{
			Log::Trace("", "", "Tele[{0}]", (const char*)epex.GetMsg());
			sprintf(s.msg, (const char*)epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		epex.Uninitialize();
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


