/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-11
Description: 低倍硫印实绩接收
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
#include "epex.h"

//程序用头文件，请包含在""中
 

/*<remark>=========================================================
/// <summary>
/// 低倍硫印实绩接收
/// 电文号200003
/// <para>
/// 获取输入参数：板坯号，检验标记 ；
/// </para>
/// <para>数据库表：TQMTS27 实绩_低倍硫印       
/// </para>
/// </summary>
/// <param name="SLAB_NO">板坯号			</param>
/// <param name="ISE_TEST_FLAG">检验标记    </param>
===========================================================</remark>*/  

// service入口
BM2F_ENTERACE_TELE(cm_200003_rcv)


int f_cm_200003_rcv(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn) 
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	CString op_flag = "";

	CString sqlstr = "";

	/* 实体类定义 */
	CModel tqmts27("TQMTS27");
	
	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{		
		tqmts27.Reset();
		tqmts27.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		op_flag = bcls_rec->Tables[0].Rows[0]["op_flag"].ToString().Trim();

		Log::Trace("", "","f_qm200003_snd IN:---op_flag = [{0}]",(const char*)op_flag);
		Log::Trace("", "","f_qm200003_snd IN:---SLAB_NO = [{0}]",(const char*)tqmts27["SLAB_NO"].ToString());
		Log::Trace("", "","f_qm200003_snd IN:---ISE_TEST_FLAG = [{0}]",tqmts27["ISE_TEST_FLAG"].ToDecimal().ToInt32());

		if (tqmts27["SLAB_NO"].ToString().Trim() == "")
		{
			strcpy(s.msg,_RES("GCRSS0000035")/*材料号不能为空*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//删除原有低倍硫印实绩
		tqmts27.Delete("SLAB_NO,ISE_TEST_FLAG"); //条件字段项

		if(op_flag != "0")
		{
			tqmts27["REC_CREATOR"] = "200003";
			tqmts27["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tqmts27["REC_REVISOR"] = " ";
			tqmts27["REC_REVISE_TIME"] = " ";
			tqmts27["ARCHIVE_FLAG"] = " ";

			//写入低倍硫印实绩
			tqmts27.Insert();
		}
		else
		{
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
