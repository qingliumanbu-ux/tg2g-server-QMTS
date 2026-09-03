/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-03-27
Description: 修改制造标准
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中

/*<remark>=========================================================
/// <summary>
/// 修改制造标准
/// <para>
/// 获取输入参数：table_name(制造标准表名)/st_no(出钢记号)/whole_backlog_code(炼钢工序)
/// </para>
/// <para>数据库表：TQMTS0x(各个制造标准表) 
/// 				TQMTS01(工序制造标准表)
/// 前台各个QMTS0x画面的F3(修改)调用    </para>
/// </summary>
/// <param name="TQMTS0x">各个制造标准表    </param>
===========================================================</remark>*/ 

/* ***** 外部函数申明 ***** */
int f_pssm_stno_chk_using(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn); //校验出钢记号是否在计划中使用

// service入口
BM2F_ENTERACE(qmts_updstd)

int f_qmts_updstd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn) 
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	CString sqlstr("");
	int doFlag = 0;
	int i;

	CString table_name = " ";
	CString s_st_no = " ";

	CString sql = "";
	CString sql_upd = "";

	EIClass bcls_rec_s;
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING,"FACTORY_DIV");
	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[1].Columns.Add(DT_STRING,"ST_IDX_NO");

	/* 实体类定义 */

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		table_name	= bcls_rec->Tables[1].Rows[0]["table_name"].ToString().Trim();
		s_st_no		= bcls_rec->Tables[1].Rows[0]["st_no"].ToString().Trim();

		Log::Trace("", "", "qmts_updstd IN: table_name[{0}]st_no[{1}]", (const char*)table_name, (const char*)s_st_no);

		if (s_st_no.TrimOrBlank() == " ")
		{
			strcpy(s.msg, _RES("QM00S0006260")/*内部钢种不能为空。*/);
			throw CApplicationException(-1, s.msg, s.svc_name);
		}

		//检查出钢记号是否在计划中使用
		//bcls_rec_s.Tables[0].Rows.Add();
		//bcls_rec_s.Tables[0].Rows[0]["FACTORY_DIV"] = "A";
		//bcls_rec_s.Tables[1].Rows.Add();
		//bcls_rec_s.Tables[1].Rows[0]["ST_IDX_NO"] = s_st_no;
		//doFlag = f_pssm_stno_chk_using(&bcls_rec_s, bcls_ret, conn);
		//if (doFlag != 0)
		//{
		//	throw CApplicationException(-1, s.msg, log.Location);
		//}

		//拼修改语句
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			sql_upd = "UPDATE " + table_name + " SET ";

			for (int j = 0; j < bcls_rec->Tables[0].Columns.get_Count(); j++)
			{
				//修改SQL
				sql_upd += bcls_rec->Tables[0].Columns[j].get_ColumnName() + " = '" + (CString)bcls_rec->Tables[0].Rows[i][j].ToString().TrimOrBlank() + "'";

				if (j != bcls_rec->Tables[0].Columns.get_Count() - 1)
					sql_upd += ",";

				Log::Trace("", "", "sql_upd = [{0}]", (const char*)sql_upd);
			}

			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sql = sql_upd + " where st_no = @st_no ";
				break;
			}

			Log::Trace("", "", "sql = [{0}]", (const char*)sql);

			//连接数据库
			CDbCommand comm1(sql, conn);
			comm1.Parameters.Set("st_no", s_st_no);
			comm1.ExecuteNonQuery();
			comm1.Close();

			//修改新增责任人，时间
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
			default:
				sql = "UPDATE " + table_name
					+ " SET REC_REVISOR = @rec_revisor , "
					+ " REC_REVISE_TIME = @rec_revise_time , "
					+ " DU_FLAG = 'U' , "
					+ " DU_MAKER = @rec_revisor , "
					+ " DU_TIME = @rec_revise_time "
					+ " WHERE ST_NO = @st_no "
					;
				break;
			}

			//连接数据库
			Log::Trace("", "", "sql[{0}]", (const char *)sql);

			CDbCommand comm2(sql, conn);
			comm2.Parameters.Set("st_no", s_st_no);
			comm2.Parameters.Set("rec_revisor", s.userid);
			comm2.Parameters.Set("rec_revise_time", CDateTime::Now().ToString("yyyyMMddHHmmss"));
			comm2.ExecuteNonQuery();
			comm2.Close();

		}
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
