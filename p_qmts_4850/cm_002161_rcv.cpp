/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      潘陈
Version:     1.0
Date:        2023-12-11 15:08:16
Description: 合同信息接收
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2F_ENTERACE_TELE(cm_002161_rcv)

int f_cm_002161_rcv(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CModel tqmom01("TQMOM01");
	CModel tqmom02("TQMOM02");
	CModel tqmom03("TQMOM03");
	try
	{

		tqmom01.MergeFrom(bcls_rec->Tables["bapiheader"].Rows[0]);

		tqmom01["ORDER_NO"] = bcls_rec->Tables["zcho_pp_lghtxf"].Rows[0]["order_no"].ToString();
		tqmom01.Delete("ORDER_NO");

		for (int i = 0; i < bcls_rec->Tables["zcho_pp_lghtxf"].Rows.get_Count(); i++)
		{

			tqmom01.MergeFrom(bcls_rec->Tables["zcho_pp_lghtxf"].Rows[i]);
			tqmom01["SAP_ERP_AUFNR"] = bcls_rec->Tables["zcho_pp_lghtxf"].Rows[i]["order_no"].ToString();
			tqmom01["MSC_SRC"] = bcls_rec->Tables["zcho_pp_lghtxf"].Rows[i]["msc"].ToString();

			tqmom01.TrimOrBlank();
			tqmom01.Insert();
		}

		tqmom02["ORDER_NO"] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[0]["aufnr"].ToString();
		tqmom02.Delete("ORDER_NO");
		for (int i = 0; i < bcls_rec->Tables["zcho_pp_lghtsx"].Rows.get_Count(); i++)
		{
			//订单号(合同号)
			tqmom02["ORDER_NO"] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[i]["aufnr"].ToString();
			//特性名称
			tqmom02["ATTRI_ITEM"] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[i]["atnam"].ToString();
			//特性值
			tqmom02["ATTRI_TEXT"] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[i]["atwrt"].ToString();
			//特性值文本
			tqmom02["ATWTB_ZL"] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[i]["atwtb"].ToString();
			//内部浮点自
			tqmom02["ATTRI_NUM_ATFLV"] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[i]["atflv"];
			//内部浮点至
			tqmom02["ATTRI_NUM_ATFLB"] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[i]["atflb"];

			tqmom02["TIMESTAMP"] = bcls_rec->Tables["zcho_pp_lghtsx"].Rows[i]["timestamp"].ToString();
			tqmom02.TrimOrBlank();
			tqmom02.Insert();
		}

		tqmom03["ORDER_NO"] = bcls_rec->Tables["zcho_pp_lghtzc"].Rows[0]["order_no"];
		tqmom03.Delete("ORDER_NO");
		for (int i = 0; i < bcls_rec->Tables["zcho_pp_lghtzc"].Rows.get_Count(); i++)
		{
			//合同号
			tqmom03["ORDER_NO"] = bcls_rec->Tables["zcho_pp_lghtzc"].Rows[i]["order_no"];
			//制程号
			tqmom03["ALPHA_CODE"] = bcls_rec->Tables["zcho_pp_lghtzc"].Rows[i]["line_no"];
			//全程工序
			tqmom03["WHOLE_BACKLOG"] = bcls_rec->Tables["zcho_pp_lghtzc"].Rows[i]["whole_backlog"];
			tqmom03.TrimOrBlank();
			tqmom03.Insert();
		}

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
