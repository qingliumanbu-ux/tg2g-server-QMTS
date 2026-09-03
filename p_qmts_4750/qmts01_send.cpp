/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   刘家岩
Version:    1.0
Date:     2012-02-16
Description: 工艺卡确认
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中


/*<remark>=========================================================
/// <summary>
/// 工艺卡确认
/// <para>
/// 获取输入参数：TQMTS01(工序制造标准表) ；
/// </para>
/// <para>数据库表：TQMTS01(工序制造标准表)); 
///  前台QMTS01画面的F11(下发)调用    </para>
/// </summary>
/// <param name="TQMTS01">工序制造标准表    </param>
===========================================================</remark>*/  

// service入口
BM2F_ENTERACE(qmts01_send)

int f_qmts01_send(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	CString sqlstr("");
	CString base_code = "";
	int doFlag = 0;
	int i;

	CDecimal v_count;

	EIClass bcls_rec_s;
	EIClass bcls_ret_s;

	bcls_rec_s.Tables.Add();
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "st_no");
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "factory_div");
	bcls_rec_s.Tables[0].Columns.Add(DT_STRING, "whole_backlog_code");
	bcls_rec_s.Tables[0].Rows.Add();
	
	/* 实体类定义 */
	CModel tqmts01("TQMTS01");
	
	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{  
		base_code = bcls_rec->Tables[1].Rows[0]["base_code"].ToString().Trim();
		//对输入信息循环处理
		for (i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++ )
		{
			//取得单行传入信息
			tqmts01.Reset();
			tqmts01.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			Log::Trace("", "", "qmts01_send IN: st_no=[{0}], whole_backlog_code=[{1}]FACTORY_DIV[{2}]", (const char*)tqmts01["ST_NO"].ToString(), (const char*)tqmts01["WHOLE_BACKLOG_CODE"].ToString(), tqmts01["FACTORY_DIV"].ToString());

			if(tqmts01["ST_NO"].ToString().Trim() == "")
			{
				sprintf(s.msg,_RES("QM00S0004171")/*出钢记号不可为空。*/);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if(tqmts01["WHOLE_BACKLOG_CODE"].ToString().Trim() == "")
			{
				strcpy(s.msg,_RES("QM00S0004177")/*工序代码不能为空。*/);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = " SELECT IF_PASS "
						"   FROM TQMTS01 "
						"  WHERE ST_NO = @st_no "
						"    AND WHOLE_BACKLOG_CODE = @whole_backlog_code"
						"　AND FACTORY_DIV=@factory_div"
						" AND BASE_CODE =@base_code"
						;
					break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("st_no", tqmts01["ST_NO"].ToString());
			cmd_inq.Parameters.Set("whole_backlog_code", tqmts01["WHOLE_BACKLOG_CODE"].ToString());
			cmd_inq.Parameters.Set("factory_div", tqmts01["FACTORY_DIV"].ToString());
			cmd_inq.Parameters.Set("base_code", base_code);
			cmd_inq.ExecuteReader();
			if(cmd_inq.Read())
			{
				tqmts01["IF_PASS"] = cmd_inq.GetString(1);
			}
			cmd_inq.Close();
			if (tqmts01["IF_PASS"].ToString() != "1")
			{
				sprintf(s.msg,"该工序出钢记号[{0}]的制造标准未生效，不可送信",(const char*)tqmts01["ST_NO"].ToString());
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//生效工序对应的制造标准必须存在
			if (tqmts01["WHOLE_BACKLOG_CODE"].ToString() == "S")
			{
				switch(conn->DatabaseKind)
				{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:						// 所有数据库适用，通用SQL语句
						sqlstr = "SELECT COUNT(1) "
							"  FROM TQMTS03 "
							" WHERE ST_NO = @st_no "
							"　AND FACTORY_DIV=@factory_div"
							" AND BASE_CODE =@base_code"
							;
						break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("st_no", tqmts01["ST_NO"].ToString());
				cmd_inq.Parameters.Set("factory_div", tqmts01["FACTORY_DIV"].ToString());
				cmd_inq.Parameters.Set("base_code", base_code);
				v_count = cmd_inq.ExecuteScalar();
				if (v_count == 0)
				{
					sprintf(s.msg,_RES("QM00S0003983")/*铁水预处理工序无出钢记号的制造标准，不能送信。*/,(const char*)tqmts01["ST_NO"].ToString());
					throw CApplicationException(-1, s.msg, log.Location);
				}
				else  //
				{
					//调用发送电文函数
					bcls_rec_s.Tables[0].Rows[i]["ST_NO"] = tqmts01["ST_NO"].ToString();
					bcls_rec_s.Tables[0].Rows[i]["FACTORY_DIV"] = tqmts01["FACTORY_DIV"].ToString();  //tqmts01.FACTORY_DIV
					bcls_rec_s.Tables[0].Rows[i]["WHOLE_BACKLOG_CODE"] = tqmts01["WHOLE_BACKLOG_CODE"].ToString();  //tqmts01.WHOLE_BACKLOG_CODE
					//doFlag = f_cm_0a1b41_snd(&bcls_rec_s, &bcls_ret_s, conn);
					if (doFlag != 0)
					{
						sprintf(s.msg, "f_cm_0a1b41_snd调用发送电文函数失败! doFlag = [%s]", doFlag);
						throw CApplicationException(-1, s.msg, log.Location);
					}

				}
			}
			if (tqmts01["WHOLE_BACKLOG_CODE"].ToString() == "B")
			{
				switch(conn->DatabaseKind)
				{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:						// 所有数据库适用，通用SQL语句
						sqlstr = "SELECT COUNT(1) "
								 "  FROM TQMTS04 "
								 " WHERE ST_NO = @st_no "
								 "　AND FACTORY_DIV=@factory_div"
						        	" AND BASE_CODE =@base_code"
								 ;
						break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("st_no", tqmts01["ST_NO"].ToString());
				cmd_inq.Parameters.Set("factory_div", tqmts01["FACTORY_DIV"].ToString());
				cmd_inq.Parameters.Set("base_code", base_code);
				v_count = cmd_inq.ExecuteScalar();
				if (v_count == 0)
				{
					sprintf(s.msg,_RES("QM00S0003984")/*转炉工序无出钢记号的制造标准，不能送信。*/,(const char*)tqmts01["ST_NO"].ToString());
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			if (tqmts01["WHOLE_BACKLOG_CODE"].ToString() == "V")
			{
				switch(conn->DatabaseKind)
				{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:						// 所有数据库适用，通用SQL语句
						sqlstr = "SELECT COUNT(1) "
								 "  FROM TQMTS05 "
								 " WHERE ST_NO = @st_no "
								 "　AND FACTORY_DIV=@factory_div"
							     " AND BASE_CODE =@base_code"
								 ;
						break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("st_no", tqmts01["ST_NO"].ToString());
				cmd_inq.Parameters.Set("factory_div", tqmts01["FACTORY_DIV"].ToString());
				cmd_inq.Parameters.Set("base_code", base_code);
				v_count = cmd_inq.ExecuteScalar();
				if (v_count == 0)
				{
					sprintf(s.msg,_RES("QM00S0004023")/*VD/VOD工序无出钢记号的制造标准，不能送信。*/,(const char*)tqmts01["ST_NO"].ToString());
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			if (tqmts01["WHOLE_BACKLOG_CODE"].ToString() == "R")
			{
				switch(conn->DatabaseKind)
				{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:						// 所有数据库适用，通用SQL语句
						sqlstr = "SELECT COUNT(1) "
								 "  FROM TQMTS06 "
								 " WHERE ST_NO = @st_no "
								 "　AND FACTORY_DIV=@factory_div"
							      " AND BASE_CODE =@base_code"
								 ;
						break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("st_no", tqmts01["ST_NO"].ToString());
				cmd_inq.Parameters.Set("factory_div", tqmts01["FACTORY_DIV"].ToString());
				cmd_inq.Parameters.Set("base_code", base_code);
				v_count = cmd_inq.ExecuteScalar();
				if (v_count == 0)
				{
					sprintf(s.msg,_RES("QM00S0003987")/*RH工序无出钢记号的制造标准，不能送信。*/,(const char*)tqmts01["ST_NO"].ToString());
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			if (tqmts01["WHOLE_BACKLOG_CODE"].ToString() == "L")
			{
				switch(conn->DatabaseKind)
				{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:						// 所有数据库适用，通用SQL语句
						sqlstr = "SELECT COUNT(1) "
								 "  FROM TQMTS07 "
								 " WHERE ST_NO = @st_no "
								 "　AND FACTORY_DIV=@factory_div"
							" AND BASE_CODE =@base_code"
								 ;
						break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("st_no", tqmts01["ST_NO"].ToString());
				cmd_inq.Parameters.Set("factory_div", tqmts01["FACTORY_DIV"].ToString());
				cmd_inq.Parameters.Set("base_code", base_code);
				v_count = cmd_inq.ExecuteScalar();
				if (v_count == 0)
				{
					sprintf(s.msg,_RES("QM00S0003986")/*LF工序无出钢记号的制造标准，不能送信。*/,(const char*)tqmts01["ST_NO"].ToString());
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			if (tqmts01["WHOLE_BACKLOG_CODE"].ToString() == "C")
			{
				switch(conn->DatabaseKind)
				{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:						// 所有数据库适用，通用SQL语句
						sqlstr = "SELECT COUNT(1) "
								 "  FROM TQMTS08 "
								 " WHERE ST_NO = @st_no "
								 "　AND FACTORY_DIV=@factory_div"
							" AND BASE_CODE =@base_code"
								 ;
						break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("st_no", tqmts01["ST_NO"].ToString());
				cmd_inq.Parameters.Set("factory_div", tqmts01["FACTORY_DIV"].ToString());
				cmd_inq.Parameters.Set("base_code", base_code);
				v_count = cmd_inq.ExecuteScalar();
				if (v_count == 0)
				{
					sprintf(s.msg,_RES("QM00S0003988")/*连铸工序无出钢记号的制造标准，不能送信。*/,(const char*)tqmts01["ST_NO"].ToString());
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			if (tqmts01["WHOLE_BACKLOG_CODE"].ToString() == "E")
			{
				switch(conn->DatabaseKind)
				{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:						// 所有数据库适用，通用SQL语句
						sqlstr = "SELECT COUNT(1) "
								 "  FROM TQMTS0A "
								 " WHERE ST_NO = @st_no "
								 "　AND FACTORY_DIV=@factory_div"
							" AND BASE_CODE =@base_code"
								 ;
						break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("st_no", tqmts01["ST_NO"].ToString());
				cmd_inq.Parameters.Set("factory_div", tqmts01["FACTORY_DIV"].ToString());
				cmd_inq.Parameters.Set("base_code", base_code);
				v_count = cmd_inq.ExecuteScalar();
				if (v_count == 0)
				{
					sprintf(s.msg,_RES("QM00S0004048")/*电炉工序无出钢记号的制造标准，不能送信。*/,(const char*)tqmts01["ST_NO"].ToString());
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			if (tqmts01["WHOLE_BACKLOG_CODE"].ToString() == "I")
			{
				switch(conn->DatabaseKind)
				{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:						// 所有数据库适用，通用SQL语句
						sqlstr = "SELECT COUNT(1) "
								 "  FROM TQMTS0M "
								 " WHERE ST_NO = @st_no "
								 "　AND FACTORY_DIV=@factory_div"
							" AND BASE_CODE =@base_code"
								 ;
						break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("st_no", tqmts01["ST_NO"].ToString());
				cmd_inq.Parameters.Set("factory_div", tqmts01["FACTORY_DIV"].ToString());
				cmd_inq.Parameters.Set("base_code", base_code);
				v_count = cmd_inq.ExecuteScalar();
				if (v_count == 0)
				{
					sprintf(s.msg,_RES("QM00S0006061")/*模铸工序无出钢记号的制造标准，不能送信。*/,(const char*)tqmts01["ST_NO"].ToString());
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
		 
			tqmts01["IF_MESSAGE"] = "1";
			tqmts01["MESSAGE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tqmts01.Update("IF_MESSAGE, MESSAGE_TIME",  //修改字段项
				"ST_NO, WHOLE_BACKLOG_CODE,FACTORY_DIV"); //条件字段项
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
