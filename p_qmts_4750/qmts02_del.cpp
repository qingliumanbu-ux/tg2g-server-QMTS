/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-12
Description: 工序成分标准删除
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中


/*<remark>=========================================================
/// <summary>
/// 工序成分标准删除
/// <para>
/// 获取输入参数：TQMTS02(成分标准) ；
/// </para>
/// <para>数据库表：TQMTS02(成分标准)); 
///  前台画面QMTS02成分标准F6(删除)调用  </para>
/// </summary>
/// <param name="TQMTS02">成分标准    </param>
===========================================================</remark>*/  

/* ***** 外部函数申明 ***** */
int f_pssm_stno_chk_using(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn); //校验出钢记号是否在计划中使用

// service入口
BM2F_ENTERACE(qmts02_del)

int f_qmts02_del(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	EIClass bcls_rec_s;
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING,"FACTORY_DIV");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[1].Columns.Add(DT_STRING,"ST_IDX_NO");

	CString sqlstr = "";
	CString base_code = "";
	
	/* 实体类定义 */
	CModel tqmts02("TQMTS02");

	try
	{
		//对输入信息循环处理
		for (i = 1; i <= bcls_rec->Tables[0].Rows.get_Count(); i++ )
		{
			//取得单行传入信息
			tqmts02.Reset();
			tqmts02.MergeFrom(bcls_rec->Tables[0].Rows[i-1]);
			base_code = bcls_rec->Tables[1].Rows[0]["base_code"].ToString().Trim();
			Log::Trace("", "","qmts02_del IN:---ST_NO = [{0}]",(const char*)tqmts02["ST_NO"].ToString());
			Log::Trace("", "","qmts02_del IN:---FACTORY_DIV = [{0}]",(const char*)tqmts02["FACTORY_DIV"].ToString());
			Log::Trace("", "","qmts02_del IN:---WHOLE_BACKLOG_CODE = [{0}]",(const char*)tqmts02["WHOLE_BACKLOG_CODE"].ToString());
			Log::Trace("", "", "qmts02_upd IN:---BASE_CODE = [{0}]", (const char*)base_code);

			if(tqmts02["ST_NO"].ToString().Trim() == "" || tqmts02["ST_NO"].ToString().Trim() == " ")
			{
				sprintf(s.msg,_RES("QM00S0004171")/*出钢记号不可为空。*/);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//if(tqmts02["FACTORY_DIV"].ToString().Trim() == "" || tqmts02["FACTORY_DIV"].ToString().Trim() == " ")
			//{
			//	sprintf(s.msg,_RES("QM00S0004175")/*传入厂别区分不可为空。*/);
			//	throw CApplicationException(-1, s.msg, log.Location);
			//}
			//if(tqmts02["WHOLE_BACKLOG_CODE"].ToString().Trim() == "" || tqmts02["WHOLE_BACKLOG_CODE"].ToString().Trim() == " ")
			//{
			//	sprintf(s.msg,_RES("QM00S0004177")/*工序代码不能为空。*/);
			//	throw CApplicationException(-1, s.msg, log.Location);
			//}

			//调用检查出钢记号是否在计划中使用的函数
			bcls_rec_s.Tables[0].Rows.Add();
			bcls_rec_s.Tables[0].Rows[0]["FACTORY_DIV"] = tqmts02["FACTORY_DIV"];
			bcls_rec_s.Tables[1].Rows.Add();
			bcls_rec_s.Tables[1].Rows[0]["ST_IDX_NO"] = tqmts02["ST_NO"];
			doFlag = f_pssm_stno_chk_using(&bcls_rec_s, bcls_ret,conn);
			if(doFlag != 0)
			{    
				throw CApplicationException(-1, s.msg, log.Location);
			}
			tqmts02["BASE_CODE"] = base_code;
			tqmts02.Delete("ST_NO, WHOLE_BACKLOG_CODE, FACTORY_DIV,BASE_CODE"); //条件字段项
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
