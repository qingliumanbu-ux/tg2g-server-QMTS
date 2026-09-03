/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   刘家岩
Version:    1.0
Date:     2012-02-16
Description: 工艺卡确认画面修改
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中



/*<remark>=========================================================
/// <summary>
/// 工艺卡确认画面修改
/// <para>
/// 获取输入参数：TQMTS01(工序制造标准表) ；
/// </para>
/// <para>数据库表：TQMTS01(工序制造标准表)); 
///  前台QMTS01画面的F4(修改)调用   </para>
/// </summary>
/// <param name="TQMTS01">工序制造标准表    </param>
===========================================================</remark>*/  

// service入口
BM2F_ENTERACE(qmts01_upd)

int f_qmts01_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	CString sqlstr("");
	int doFlag = 0;
	int i;
	
	/* 实体类定义 */
	CModel tqmts01("TQMTS01");
	CModel tqmts02("TQMTS02");
	CModel tqmts0x("TQMTS0X");
	try
	{
		//对输入信息循环处理
		for (i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++ )
		{		    
			//取得单行传入信息
			tqmts01.Reset();
			tqmts01.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			Log::Trace("", "", "qmts01_upd IN: st_no[{0}]tqmts01.FACTORY_DIV[{1}]", (const char*)tqmts01["ST_NO"].ToString(), tqmts01["FACTORY_DIV"].ToString());

			if(tqmts01["ST_NO"].ToString().Trim() == "")
			{
				strcpy(s.msg,_RES("QM00S0004171")/*出钢记号不可为空。*/);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if(tqmts01["WHOLE_BACKLOG_SEQ"].ToDecimal() == 0)
			{
				strcpy(s.msg,_RES("QM00S0004180")/*工序顺序不可为空或0。*/);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			tqmts0x["ST_NO"] = tqmts01["ST_NO"];
			tqmts0x["FACTORY_DIV"] = tqmts01["FACTORY_DIV"];
			tqmts0x["BASE_CODE"] = tqmts01["BASE_CODE"];
			tqmts0x.Query("ST_NO,FACTORY_DIV,BASE_CODE");;
			tqmts01["REC_REVISOR"] = s.userid;
			tqmts01["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tqmts01["FACTORY_DIV"] = tqmts0x["FACTORY_DIV"];
			//tqmts01["MESSAGE_TIME"] = " ";
			if(tqmts01["IF_PLAN"].ToString() != "1")
			{
				tqmts01["IF_PLAN"] = "0";
			}
			if (tqmts01["IF_PASS"].ToString() != "1")
			{
				tqmts01["IF_PASS"] = "0";
			}
			if (tqmts01["IF_MESSAGE"].ToString() != "1")
			{
				tqmts01["IF_MESSAGE"] = "0";
			}
			tqmts01.Update("IF_PLAN, IF_PASS, IF_MESSAGE, MESSAGE_TIME, WHOLE_BACKLOG_SEQ, REC_REVISOR, REC_REVISE_TIME,RES_CODE,FIN_CONFM_TIME,FIN_CONFM_MAKER,STD_ROUTE,ST_IDX_NO,TABLE_ITEM_SEQ",  //修改字段项
				"ST_NO,FACTORY_DIV, WHOLE_BACKLOG_CODE,BASE_CODE"); //条件字段项

			tqmts02["WHOLE_BACKLOG_SEQ"] = tqmts01["WHOLE_BACKLOG_SEQ"];
			tqmts02["FACTORY_DIV"] = tqmts01["FACTORY_DIV"];
			tqmts02["ST_NO"] = tqmts01["ST_NO"];
			tqmts02["WHOLE_BACKLOG_CODE"] = tqmts01["WHOLE_BACKLOG_CODE"];
			tqmts02["BASE_CODE"] = tqmts01["BASE_CODE"];
			tqmts02.Update("WHOLE_BACKLOG_SEQ",  //修改字段项
							"FACTORY_DIV, ST_NO, WHOLE_BACKLOG_CODE,BASE_CODE"); //条件字段项
		}
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	
	return doFlag;
}
