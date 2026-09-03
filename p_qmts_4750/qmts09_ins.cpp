/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-12
Description: 新增混杂元素管理
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中



/*<remark>=========================================================
/// <summary>
/// 新增混杂元素管理
/// <para>
/// 获取输入参数：TQMTS09(混杂元素管理表) ；
/// </para>
/// <para>数据库表：TQMTS09(混杂元素管理表)); 
///               
/// 前台QMTS09画面的F3(新增)调用    </para>
/// </summary>
/// <param name="TQMTS09">混杂元素管理表    </param>
===========================================================</remark>*/ 


BM2F_ENTERACE(qmts09_ins)

BM2_FUNCTION_IMPORT
int f_cm_002002_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
int f_qmts09_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;
	
	CString sqlstr = "";

	/* 实体类定义 */
	CModel tqmts09("TQMTS09");
	
	/* 数据库操作类定义：统一放在Service或函数前段 */
	EIClass bcls_rec_snd;
	EIClass bcls_ret_snd;

	try
	{
		//对输入信息循环处理
		for (i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++ )
		{   
			//取得单行传入信息
			tqmts09.Reset();
			tqmts09.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			Log::Trace("", "","qmts09_ins IN:---HZGL = [{0}]",(const char*)tqmts09["HZGL"].ToString());

			if(tqmts09["HZGL"].ToString().Trim() == "" || tqmts09["HZGL"].ToString().Trim() == " ")
			{
				sprintf(s.msg,_RES("QM00S0004169")/*混杂元素管理区分不可为空。*/);
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			/******************** 赋初值 *************************/
			tqmts09["REC_CREATOR"] = s.userid;
			tqmts09["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tqmts09["REC_REVISOR"] = " ";
			tqmts09["REC_REVISE_TIME"] = " ";
			tqmts09["ARCHIVE_FLAG"] = " ";
			tqmts09["DU_FLAG"] = " ";
			tqmts09["DU_MAKER"] = " ";
			tqmts09["DU_TIME"] = " ";
			tqmts09["VERSION"] = 1;
			tqmts09["CATCH_RESP"] = " ";
			tqmts09["CATCH_TIME"] = " ";
			tqmts09["MESSAGE_RESP"] = " ";
			tqmts09["MESSAGE_TIME"] = " ";
			tqmts09["RES_CODE"] = " ";
			tqmts09["ABN_CONT"] = " ";
			tqmts09["TC_TRANS_TIME"] = " ";

			tqmts09.TrimOrBlank();
			tqmts09.Insert();
#ifdef _SYS_MMS   /*是MMS系统时，发送电文到PES*/
			Log::Trace("", "", "调用函数f_cm_002002_snd");
			tqmts09.MergeTo(bcls_rec_snd.Tables[0], false);
			if (!bcls_rec_snd.Tables[0].Columns.Contains("FLAG"))
			{
				bcls_rec_snd.Tables[0].Columns.Add(DT_STRING, "FLAG");
			}
			if (bcls_rec_snd.Tables[0].Rows.get_Count() == 0)
			{
				bcls_rec_snd.Tables[0].Rows.Add();
			}
			bcls_rec_snd.Tables[0].Rows[0]["FLAG"] = "1";
			doFlag = f_cm_002002_snd(&bcls_rec_snd, &bcls_ret_snd,conn);
			if (doFlag != 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
#endif
		}
		sprintf(s.msg,_RES("GCRSS0000018")/*新增信息失败。*/);
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
