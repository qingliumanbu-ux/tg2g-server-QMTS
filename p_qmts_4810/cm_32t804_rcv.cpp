/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      WSL
Version:     1.0
Date:        2023-11-29 11:08:16
Description: 铁水质量数据接收
**************************************************/
#include "stdafx.h"
#include "epex.h"
#include "CUtils.h"

BM2F_ENTERACE_TELE(cm_32t804_rcv)

int f_cm_32t804_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CModel tqmts24s("TQMTS24S");
	CModel tqmts25s("TQMTS25S");
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	try
	{
		//操作标记
		tqmts24s["DEAL_FLAG"] = bcls_rec->Tables[0].Rows[0]["DEAL_FLAG"].ToString();
		Log::Trace("", "DEAL_FLAG", "DEAL_FLAG = {0}", tqmts24s["DEAL_FLAG"]);
		//质检单号
		tqmts24s["SAMPLE_NO"] = bcls_rec->Tables[0].Rows[0]["SAMPLE_NO"].ToString();
		//质检委托号
		tqmts24s["SAMPLE_ENTR_NO"] = bcls_rec->Tables[0].Rows[0]["SAMPLE_ENTR_NO"].ToString();
		//样本编号
		tqmts24s["SAMPLE_NO_1"] = bcls_rec->Tables[0].Rows[0]["SAMPLE_NO_1"].ToString();
		//质检类型
		tqmts24s["INSPECT_TYPE"] = bcls_rec->Tables[0].Rows[0]["INSPECT_TYPE"].ToString();
		//采购订单号
		tqmts24s["C_ORDERID"] = bcls_rec->Tables[0].Rows[0]["BUY_ORDER_NO"].ToString();
		//批次号
		tqmts24s["BATCH_NO"] = bcls_rec->Tables[0].Rows[0]["BATCH_NO"].ToString();
		//物料编码
		tqmts24s["MAT_CODE"] = bcls_rec->Tables[0].Rows[0]["MAT_CODE"].ToString();
		//物料名称
		tqmts24s["MAT_CNAME"] = bcls_rec->Tables[0].Rows[0]["MAT_CNAME"].ToString();
		//取样地点代码
		tqmts24s["SAMPLE_POS"] = bcls_rec->Tables[0].Rows[0]["SAMPLE_POS_CODE"].ToString();
		//取样地点名称
		tqmts24s["SAMPLE_PURPOSE"] = bcls_rec->Tables[0].Rows[0]["SAMPLE_POS_CNAME"].ToString();
		//取样时刻
		tqmts24s["SAMPLE_TIME"] = bcls_rec->Tables[0].Rows[0]["SAMPLE_TIME"].ToString();
		//质检标准
		tqmts24s["INSPECT_REQ"] = bcls_rec->Tables[0].Rows[0]["INSPECT_STD"].ToString();
		//分析时刻
		tqmts24s["ANALYSE_TIME"] = bcls_rec->Tables[0].Rows[0]["ANALYSE_TIME"].ToString();
		//分析责任者
		tqmts24s["AYL_MAKE"] = bcls_rec->Tables[0].Rows[0]["ANALYSE_BY"].ToString();
		//质检说明
		tqmts24s["ANALYSE_REMARK"] = bcls_rec->Tables[0].Rows[0]["ANALYSE_REMARK"].ToString();
		//备用1
		tqmts24s["REMARK_1"] = bcls_rec->Tables[0].Rows[0]["BACK1"].ToString();
		//备用2
		tqmts24s["REMARK_2"] = bcls_rec->Tables[0].Rows[0]["BACK2"].ToString();
		//备用3
		tqmts24s["REMARK_3"] = bcls_rec->Tables[0].Rows[0]["BACK3"].ToString();

		//备用4
		tqmts24s["REMARK_4"] = bcls_rec->Tables[0].Rows[0]["BACK4"].ToString();
		//备用5
		tqmts24s["REMARK_5"] = bcls_rec->Tables[0].Rows[0]["BACK5"].ToString();
		

		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			//质检委托号
			tqmts25s["SAMPLE_ENTR_NO"] = bcls_rec->Tables[0].Rows[i]["SAMPLE_ENTR_NO"].ToString();
			Log::Trace("", "SAMPLE_ENTR_NO", "SAMPLE_ENTR_NO = {0}", tqmts25s["SAMPLE_ENTR_NO"]);
			//分析项目数
			tqmts25s["ANALYSE_ITEM_COUNT"] = bcls_rec->Tables[0].Rows[i]["ANALYSE_ITEM_NUM"].ToString();
			//分析项目代码
			tqmts25s["ITEM_DESC"] = bcls_rec->Tables[0].Rows[i]["ANALYSE_ITEM_CODE"].ToString();
			//分析项目中文
			tqmts25s["PROJECT_CNAME"] = bcls_rec->Tables[0].Rows[i]["ANALYSE_ITEM_CNAME"].ToString();
			//分析项目数据类型
			tqmts25s["ANALYSE_DATA_TYPE"] = bcls_rec->Tables[0].Rows[i]["ANALYSE_DATA_TYPE"].ToString();
			//分析项目单位
			tqmts25s["PROJECT_NO"] = bcls_rec->Tables[0].Rows[i]["ANALYSE_DATA_UNIT"].ToString();
			//分析项目值
			tqmts25s["ITEM_TEXT"] = bcls_rec->Tables[0].Rows[i]["ANALYSE_ITEM_VALUE"].ToString();
			Log::Trace("", "ITEM_TEXT", "ITEM_TEXT = {0}", tqmts25s["ITEM_TEXT"]);

			tqmts25s["REC_CREATOR"] = s.userid;
			tqmts25s["REC_CREATE_TIME"] = datetime;
			tqmts25s.Insert();
		}

		tqmts24s["REC_CREATOR"] = s.userid;
		tqmts24s["REC_CREATE_TIME"] = datetime;
		tqmts24s.Insert();
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


