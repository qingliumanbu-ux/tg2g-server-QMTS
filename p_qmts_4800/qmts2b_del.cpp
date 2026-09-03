/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-26
Description: 板坯钻样成分实绩信息删除
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中



/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
///板坯钻样成分实绩信息删除
/// <para>
/// 1.板坯钻样成分实绩信息删除
/// 
/// </para>
/// <para>数据库表：TQMTS2B(实绩_板坯成分主信息)		</para>
/// <para>          TQMTS2C(实绩_板坯成分)		        </para>
/// <para>主调用函数：前台QMTS2B画面的F5(删除)调用		</para>
/// <para>需调用函数：							        </para>
/// </summary>
/// <param name="ST_SAMPLE_NO">  试样号 				</param>
/// <returns>  </returns>
===========================================================</remark>*/

// service入口
BM2F_ENTERACE(qmts2b_del)

int f_qmts2b_del(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	/* 实体类定义 */
	CModel tqmts2b("TQMTS2B");
	CModel tqmts2c("TQMTS2C");

	CString sqlstr("");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		/* 对输入信息循环处理 */
		for (i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++ )
		{
			/* 取得单行传入信息 */
			tqmts2c.Reset();
			tqmts2c.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			Log::Trace("", "","tqmts2c_del IN:---ST_SAMPLE_NO[{0}]",(const char*)tqmts2c["ST_SAMPLE_NO"].ToString());
			
			//校验传入参数
			if(tqmts2c["ST_SAMPLE_NO"].ToString().TrimOrBlank() == " ")
			{
				strcpy(s.msg,_RES("QM00S0004260")/*请选择或输入试样号。*/);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			tqmts2c.Delete("ST_SAMPLE_NO");

			tqmts2b["ST_SAMPLE_NO"] = tqmts2c["ST_SAMPLE_NO"];
			tqmts2b.Delete("ST_SAMPLE_NO");
		}	  
		sprintf(s.msg,_RES("QM00S0004339")/*删除完毕。*/);
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
