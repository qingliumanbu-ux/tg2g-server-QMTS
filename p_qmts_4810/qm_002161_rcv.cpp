/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      WSL
Version:     1.0
Date:        2023-12-14 18:58:16
Description: 合同信息接收
**************************************************/
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"

BM2F_ENTERACE_TELE(qm_002161_rcv)

int f_qm_002161_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CModel tqmom01("TQMOM01");
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	try
	{
		tqmom01.MergeFrom(bcls_rec->Tables["bapiheader"].Rows[0]);
		
		for (int i = 0; i < bcls_rec->Tables["zcho_pp_lghtsx"].Rows.get_Count(); i++)
		{
			tqmom01["aufnr"] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[0]["aufnr"].ToString();
			tqmom01["atnam"] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[0]["atnam"].ToString();
			tqmom01["atwrt"] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[0]["atwrt"].ToString();
			tqmom01["atwtb"] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[0]["atwtb"].ToString();
			tqmom01["atflv"] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[0]["atflv"].ToString();
			tqmom01["atflb"] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[0]["atflb"].ToString();
			tqmom01["timestamp"] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[0]["timestamp"].ToString();
		}
		for (int i = 0; i < bcls_rec->Tables["zcho_pp_lghtxf"].Rows.get_Count(); i++)
		{
			tqmom01["SAP_ERP_AUFNR"] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[0]["order_no"].ToString();
			tqmom01["sale_order_no"] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[0]["sale_order_no"].ToString();
			tqmom01["item_no"] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[0]["item_no"].ToString();
			tqmom01["order_type"] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[0]["order_type"].ToString();
			tqmom01["urg_order_flag"] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[0]["urg_order_flag"].ToString();
			tqmom01["sale_order_type"] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[0]["sale_order_type"].ToString();
			tqmom01["order_modi_person"] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[0]["order_modi_person"].ToString();
			tqmom01["delivy_date"] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[0]["delivy_date"].ToString();
			tqmom01["psc"] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[0]["psc"].ToString();
			tqmom01["apn"] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[0]["apn"].ToString();
			tqmom01["apn_desc"] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[0]["apn_desc"].ToString();
			tqmom01["msc"] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[0]["msc"].ToString();
			tqmom01["sg_sign"] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[0]["sg_sign"].ToString();
			tqmom01["sg_std"] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[0]["sg_std"].ToString();
			tqmom01["prod_class_code"] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[0]["prod_class_code"].ToString();
			tqmom01["prod_class_desc"] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[0]["prod_class_desc"].ToString();
			tqmom01["surface_accu"] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[0]["surface_accu"].ToString();
			tqmom01["surface_accu_code"] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[0]["surface_accu_code"].ToString();
			tqmom01["order_thick"] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[0]["order_thick"].ToString();
			tqmom01["order_width"] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[0]["order_width"].ToString();
			tqmom01["order_len"] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[0]["order_len"].ToString();
			tqmom01["order_min_len"] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[0]["order_min_len"].ToString();
			tqmom01["order_max_len"] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[0]["order_max_len"].ToString();
			tqmom01["order_inner_dia"] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[0]["order_inner_dia"].ToString();
			tqmom01["order_outer_dia"] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[0]["order_outer_dia"].ToString();
			tqmom01[""] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[0][""].ToString();
			tqmom01[""] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[0][""].ToString();
			tqmom01[""] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[0][""].ToString();
			tqmom01[""] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[0][""].ToString();
			tqmom01[""] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[0][""].ToString();
			tqmom01[""] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[0][""].ToString();
			tqmom01[""] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[0][""].ToString();
			tqmom01[""] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[0][""].ToString();
			tqmom01[""] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[0][""].ToString();
			tqmom01[""] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[0][""].ToString();
		}
		tqmom01["order_no"] = bcls_rec->Tables["zcho_pp_lghtzc"].Rows[0]["order_no"].ToString();
		tqmom01["line_no"] = bcls_rec->Tables["zcho_pp_lghtzc"].Rows[0]["line_no"].ToString();
		tqmom01["whole_backlog"] = bcls_rec->Tables["zcho_pp_lghtzc"].Rows[0]["whole_backlog"].ToString();
		tqmom01.Insert();
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


