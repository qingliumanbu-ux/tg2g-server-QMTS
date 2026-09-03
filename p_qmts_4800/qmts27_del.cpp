/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-10
Description: 低倍硫印实绩删除
**************************************************/


//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中


/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
///低倍硫印实绩删除
/// <para>
/// 1.低倍硫印实绩删除
/// 
/// </para>
/// <para>数据库表：TQMTS27(实绩_低倍硫印)				</para>
/// <para>主调用函数：前台QMTS27画面的F5(删除)调用		</para>
/// <para>需调用函数：									</para>
/// </summary>
/// <param name="SLAB_NO">  板坯号				</param>
/// <returns>  </returns>
===========================================================</remark>*/

/* ***** 外部函数申明 ***** */
//int f_qm200003_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn);	//低倍硫印实绩上传电文

// service入口
BM2F_ENTERACE(qmts27_del)


int f_qmts27_del(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn) 
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;
	
	CString sqlstr = "";

	//调用低倍硫印实绩上传电文函数
	EIClass bcls_rec_tc;
	EIClass bcls_ret_tc;
	bcls_rec_tc.Tables[0].Columns.Add(DT_STRING,"OP_FLAG");
	bcls_rec_tc.Tables[0].Columns.Add(DT_STRING,"SLAB_NO");
	bcls_rec_tc.Tables[0].Columns.Add(DT_DECIMAL,"ISE_TEST_FLAG");
	bcls_rec_tc.Tables[0].Rows.Add();

	/* 实体类定义 */
	CModel tqmts27("TQMTS27");
	
	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_upd(conn);

	try
	{
		//对输入信息循环处理
		for (i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++ )
		{    
			//取得单行传入信息
			tqmts27.Reset();
			tqmts27.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			Log::Trace("", "","qmts27_del IN:---SLAB_NO = [{0}]",(const char*)tqmts27["SLAB_NO"].ToString());
			Log::Trace("", "","qmts27_del IN:---ISE_TEST_FLAG = [{0}]",tqmts27["ISE_TEST_FLAG"].ToDecimal().ToInt32());

			if (tqmts27["SLAB_NO"].ToString().Trim() == "")
			{
				strcpy(s.msg,_RES("GCRSS0000035")/*材料号不能为空*/);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			tqmts27.Delete("SLAB_NO,ISE_TEST_FLAG"); //条件字段项

			//总判定结果 
			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = " SELECT MIN(TO_NUMBER(JUDGE_CODE)) "
							 " FROM TQMTS27 "
							 " WHERE SLAB_NO = @slab_no ";
					break;
			}
			cmd_inq.SetCommandText(sqlstr);		
			cmd_inq.Parameters.Set("slab_no", tqmts27["SLAB_NO"].ToString());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tqmts27["ISE_FLAG"] = cmd_inq.GetDecimal(1);
			}
			Log::Trace("", "","tqmts27.ISE_FLAG[{0}]",tqmts27["ISE_FLAG"].ToDecimal().ToInt32());  

			tqmts27.Update("ISE_FLAG",  //修改字段项
							"SLAB_NO"); //条件字段项

#ifdef _SYS_PES
			Log::Trace("", "","-------发送L4低倍硫印实绩 -----!");
			bcls_rec_tc.Tables[0].Rows[0]["OP_FLAG"] = "0";
			bcls_rec_tc.Tables[0].Rows[0]["SLAB_NO"] = tqmts27["SLAB_NO"];
			bcls_rec_tc.Tables[0].Rows[0]["ISE_TEST_FLAG"] = tqmts27["ISE_TEST_FLAG"];

			//doFlag = f_qm200003_snd(&bcls_rec_tc,&bcls_ret_tc,conn);
			if(doFlag != 0)
			{
				Log::Trace("", "","f_qm200003_snd() msg = [{0}]", s.msg);
				throw CApplicationException(-1, s.msg, log.Location);
			}
#endif

			//清空物料主表中ISE_FLAG值

			//如何情况品质判定模型中的低倍硫印异常？

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
