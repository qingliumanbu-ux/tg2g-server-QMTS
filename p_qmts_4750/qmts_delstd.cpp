/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-03-27
Description: 删除制造标准
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中

/*<remark>=========================================================
/// <summary>
/// 删除制造标准
/// <para>
/// 获取输入参数：table_name(制造标准表名)/st_no(出钢记号)/whole_backlog_code(炼钢工序)
/// </para>
/// <para>数据库表：TQMTS0x(各个制造标准表) 
/// 				TQMTS01(工序制造标准表)
/// 前台各个QMTS0x画面的F3(删除)调用    </para>
/// </summary>
/// <param name="TQMTS0x">各个制造标准表    </param>
===========================================================</remark>*/ 

/* ***** 外部函数申明 ***** */
int f_pssm_stno_chk_using(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn); //校验出钢记号是否在计划中使用

// service入口
BM2F_ENTERACE(qmts_delstd)

int f_qmts_delstd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn) 
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	CString sqlstr("");
	int doFlag = 0;
	int i;

	CString table_name = " ";
	CString s_st_no = " ";

	CString sql_del = "";

	EIClass bcls_rec_s;
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING,"FACTORY_DIV");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[1].Columns.Add(DT_STRING,"ST_IDX_NO");

	/* 实体类定义 */

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		table_name	= bcls_rec->Tables[0].Rows[0]["table_name"].ToString().Trim();
		s_st_no		= bcls_rec->Tables[0].Rows[0]["st_no"].ToString().Trim();

		Log::Trace("", "","qmts_delstd IN: table_name[{0}]st_no[{1}]",(const char*)table_name,(const char*)s_st_no);

		if(s_st_no.TrimOrBlank() == " ")
		{
			strcpy(s.msg, _RES("QM00S0006260")/*内部钢种不能为空。*/);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		////检查出钢记号是否在计划中使用
		//bcls_rec_s.Tables[0].Rows.Add();
		//bcls_rec_s.Tables[0].Rows[0]["FACTORY_DIV"] = "A";
		//bcls_rec_s.Tables[1].Rows.Add();
		//bcls_rec_s.Tables[1].Rows[0]["ST_IDX_NO"] = s_st_no;
		//doFlag = f_pssm_stno_chk_using(&bcls_rec_s, bcls_ret,conn);
		//if(doFlag != 0)
		//{    
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}

		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			//删除SQL		
			sql_del = "DELETE FROM " + table_name;
			sql_del += " WHERE ST_NO = @s_st_no";

			break;
		}

		//连接数据库
		Log::Trace("", "", "sql_del[{0}]", (const char *)sql_del);

		CDbCommand comm(sql_del, conn);
		comm.Parameters.Set("s_st_no", s_st_no);
		comm.ExecuteNonQuery();
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		Log::Trace("", "","error=[{0}]", (const char*)str );

		strncpy(s.sysmsg, (const char*)str, 399);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), 399);
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), 399);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}
