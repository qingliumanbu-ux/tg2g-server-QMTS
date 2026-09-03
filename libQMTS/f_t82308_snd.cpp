/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2024-1-03 9:13:28
Description: 二钢RH冶炼操作记录电文
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2_FUNCTION_EXPORT
int f_t82308_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CModel tmmsm23("TMMSM23");
	EPEX epex;
	CDbCommand cmd_time(conn);
	CString time = "";
	try
	{
		CString epex_number = "T82308";
		CString deal_flag = bcls_rec->Tables["T823"].Rows[0]["DEAL_FLAG"].ToString().Trim();
		CString heat_no = bcls_rec->Tables["T823"].Rows[0]["HEAT_NO"].ToString().Trim();
		CString proc_no = bcls_rec->Tables["T823"].Rows[0]["PROC_NO"].ToString().Trim();
		tmmsm23["L2_PROC_NO"] = bcls_rec->Tables["T823"].Rows[0]["L2_PROC_NO"].ToString().Trim();
		if (epex.Initialize(epex_number) < 0)
		{
			sprintf(s.msg, (const char*)epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (epex.SetValue("DEAL_FLAG", 0, deal_flag) < 0)
		{
			sprintf(s.msg, (const char*)epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//获取熔炼号
		tmmsm23["HEAT_NO"] = heat_no;
		tmmsm23["PROC_NO"] = proc_no;
		tmmsm23.Query("HEAT_NO,L2_PROC_NO");
		tmmsm23.TrimOrBlank();
		if (epex.SetValue("DEAL_FLAG", 0, deal_flag) < 0)
		{
			sprintf(s.msg, (const char*)epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//工位
		if (epex.SetValue("WHOLE_BACKLOG_CODE_SM", 0, tmmsm23["DEV_CODE"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//炉号
		if (epex.SetValue("HEAT_NO", 0, tmmsm23["HEAT_NO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//处理次数
		if (epex.SetValue("PROC_COUNT", 0, tmmsm23["SAME_PROC_NUM"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//班次
		if (epex.SetValue("F_CLASS", 0, tmmsm23["PROD_SHIFT_NO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		if (tmmsm23["PROD_DATE"].ToString() == " "){
			tmmsm23["PROD_DATE"] = "19000101";
		}
		//日期
		if (epex.SetValue("DATE_TIME", 0, tmmsm23["PROD_DATE"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//班组
		if (epex.SetValue("GROUP_NO", 0, tmmsm23["PROD_SHIFT_GROUP"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//工艺路线
		/*if (epex.SetValue("WHOLE_BACKLOG", 0, tmmsm23[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/
		//下工序
		if (epex.SetValue("WHOLE_BACKLOG_ACT", 0, tmmsm23["NEXT_DEV_CODE"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//钢种牌号
		/*if (epex.SetValue("SG_SIGN", 0, tmmsm23[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/
		//钢包号
		if (epex.SetValue("STEEL_LADLE_NO", 0, tmmsm23["FURNACE_NO"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//包龄
		/*if (epex.SetValue("STEEL_LADLE_YRS", 0, tmmsm23["LADLE_AGE"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}*/
		//处理开始时刻
		if (tmmsm23["START_TIME"].ToString() == " "){
			tmmsm23["START_TIME"] = "19000101000000";
		}
		if (epex.SetValue("PROC_START_T", 0, tmmsm23["START_TIME"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//处理结束时刻
		if (tmmsm23["END_TIME"].ToString() == " "){
			tmmsm23["END_TIME"] = "19000101000000";
		}
		if (epex.SetValue("PROC_END_T", 0, tmmsm23["END_TIME"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//处理时长
		cmd_time.SetCommandText(
			" SELECT ROUND(TO_NUMBER(TO_DATE('" + tmmsm23["END_TIME"].ToString() + "', 'YYYYMMDDhh24miss') - "
			" TO_DATE('" + tmmsm23["START_TIME"].ToString() + "', 'YYYYMMDDhh24miss')) * 24 * 60 * 60) "
			" from dual ");
		cmd_time.ExecuteReader();
		if (cmd_time.Read())
		{
			time = cmd_time.GetString(1);
		}
		cmd_time.Close();
		if (epex.SetValue("PROC_TIME", 0, time) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}
		//进站钢水重量
		/*if (epex.SetValue("IN_LADLE_DEPART_WT", 0, tmmsm23[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/
		//渣厚
		/*if (epex.SetValue("SLAG_THICK3", 0, tmmsm23[""].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}*/
		//到站空间
		/*if (epex.SetValue("ARRIVE_LADLE_SPACE", 0, tmmsm23[""].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}*/
		//初次测温
		if (epex.SetValue("INI_TEMP1", 0, tmmsm23["START_STEEL_TEMP"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//真空开始时刻
		Log::Trace("", __FUNCTION__, "VAC_START=[{0}]", tmmsm23["VAC_START"].ToString());
		if (epex.SetValue("VACUUM_START_TIME", 0, tmmsm23["VAC_START"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//真空结束时间
		/*if (epex.SetValue("VACUUM_END_TIME", 0, tmmsm23[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/
		//真空时长
		/*if (epex.SetValue("AIR_TIME", 0, tmmsm23[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/
		//最低真空度
		/*if (epex.SetValue("VACUUM_DEGREE_LOW", 0, tmmsm23[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/
		//搅拌AR体积
		/*if (epex.SetValue("STIR_AR_CBM", 0, tmmsm23[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/
		//提升AR体积
		/*if (epex.SetValue("ASC_AR_CBM", 0, tmmsm23[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/
		//N2体积
		/*if (epex.SetValue("N2_CBM", 0, tmmsm23[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/
		//吹氧体积
		/*if (epex.SetValue("O2_CBM", 0, tmmsm23[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/
		//最后测温
		/*if (epex.SetValue("LAST_TEMP", 0, tmmsm23[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/
		//离站空间
		/*if (epex.SetValue("OUT_LADLE_SPACE", 0, tmmsm23[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/
		//出站渣厚
		/*if (epex.SetValue("SLAG_THICK_END1", 0, tmmsm23[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/
		//出站重量
		if (epex.SetValue("LFIMG", 0, tmmsm23["LADLE_DEPART_WT"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//备注
		if (epex.SetValue("REMARK", 0, tmmsm23["REMARK"].ToString()) < 0)
		{
			sprintf(s.msg, epex.GetMsg());
			throw CApplicationException(-1, s.msg, log.Location);
		}
		//改钢原因描述
		/*if (epex.SetValue("GG_DESC", 0, tmmsm23[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/
		//CScrap
		/*if (epex.SetValue("CSCRAP", 0, tmmsm23[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/
		//C
		/*if (epex.SetValue("C", 0, tmmsm23[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/
		//石灰量
		/*if (epex.SetValue("LIME", 0, tmmsm23[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/
		//AL
		/*if (epex.SetValue("AL", 0, tmmsm23[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/
		//ca
		/*if (epex.SetValue("CA", 0, tmmsm23[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/
		//HCFeMn
		/*if (epex.SetValue("HCFeMn", 0, tmmsm23[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/
		//als
		/*if (epex.SetValue("ALS", 0, tmmsm23[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/
		//NI
		/*if (epex.SetValue("NI", 0, tmmsm23[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/
		//高碳铬铁
		/*if (epex.SetValue("LCFECR", 0, tmmsm23[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/
		//FeNb
		/*if (epex.SetValue("FENB", 0, tmmsm23[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/
		//FeTi
		/*if (epex.SetValue("FETI", 0, tmmsm23[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/
		//FeSi
		/*if (epex.SetValue("FESI", 0, tmmsm23[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/
		//LCFeSi
		/*if (epex.SetValue("LCFESI", 0, tmmsm23[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/
		//FeV
		/*if (epex.SetValue("FEV", 0, tmmsm23[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/
		//LNFeTi
		/*if (epex.SetValue("LNFETI", 0, tmmsm23[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/
		//MCFeMn
		/*if (epex.SetValue("MCFEMN", 0, tmmsm23[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/
		//DJMn
		/*if (epex.SetValue("DJMN", 0, tmmsm23[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/
		//MnMetallic
		/*if (epex.SetValue("MNMETALLIC", 0, tmmsm23[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/
		//真空罐号
		/*if (epex.SetValue("VACUUM_NO", 0, tmmsm23[""].ToString()) < 0)
		{
		sprintf(s.msg, epex.GetMsg());
		throw CApplicationException(-1, s.msg, log.Location);
		}*/
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


