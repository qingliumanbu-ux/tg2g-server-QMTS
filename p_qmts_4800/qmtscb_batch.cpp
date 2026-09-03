/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2012
Author:     XXX
Version:    1.0
Date:       2024-09-22
Description: 标准成本批处理
**************************************************/
//框架头文件
#include "stdafx.h"


// service入口
BM2F_ENTERACE(qmtscb_batch)
int f_mmsm_zxh03(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_mmsm_zxh01(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_qmtscb_batch(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 自定义变量 ***** */
	int doFlag = 0;

	CString sqlstr = "";
	CString stat_date = CDateTime::Now().ToString("yyyyMM");
	CString end_time = CDateTime::Now().AddHours(-2).ToString("yyyyMMddHHmmss");
	//系统的分页类信息。
	CPageInfo pageInfo;
	CString v_operate = "";
	
	CDbCommand cmd_inq(conn);

	try
	{

		EIClass bcls_ret2;
		EIClass bcls_rec2;
		bcls_rec2.Tables[0].Columns.Add(DT_STRING, "STAT_DATE");
		bcls_rec2.Tables[0].Rows.Add();
		bcls_rec2.Tables[0].Rows[0]["STAT_DATE"] = stat_date;

		doFlag = f_mmsm_zxh03(&bcls_rec2, &bcls_ret2, conn);
			if (doFlag < 0)
			{
				Log::Trace("", __FUNCTION__, "-------调用f_mmsm_zxh03失败-------");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//涉及到分摊，则在2号进行重新核算
			bcls_rec2.Tables[0].Rows[0]["STAT_DATE"] = CDateTime::Now().AddDays(-3).ToString("yyyyMMddHHmmss");
			doFlag = f_mmsm_zxh01(&bcls_rec2, &bcls_ret2, conn);
			if (doFlag < 0)
			{
				Log::Trace("", __FUNCTION__, "-------调用f_mmsm_zxh01失败-------");
				throw CApplicationException(-1, s.msg, log.Location);
			}

		

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}


	return doFlag;

}
