/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-10
Description: 低倍硫印实绩新增
**************************************************/


//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中


/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
///低倍硫印实绩新增
/// <para>
/// 1.低倍硫印实绩新增
/// 
/// </para>
/// <para>数据库表：TQMTS27(实绩_低倍硫印)				</para>
/// <para>主调用函数：前台QMTS27画面的F3(新增)调用		</para>
/// <para>需调用函数：									</para>
/// </summary>
/// <param name="SLAB_NO">  板坯号				</param>
/// <returns>  </returns>
===========================================================</remark>*/

/* ***** 外部函数申明 ***** */
int f_qmts27_jud(EIClass *bcls_rec,EIClass *bcls_ret, CDbConnection *conn);//判定

// service入口
BM2F_ENTERACE(qmts27_ins)


int f_qmts27_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn) 
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	CDecimal i_count = 0;

	CString sqlstr = "";

	//调用低倍硫印判定函数
	EIClass bcls_rec_f;
	EIClass bcls_ret_f;
	bcls_rec_f.Tables[0].Columns.Add(DT_STRING,"SLAB_NO");
	bcls_rec_f.Tables[0].Columns.Add(DT_DECIMAL,"ISE_TEST_FLAG");
	bcls_rec_f.Tables[0].Rows.Add();

	/* 实体类定义 */
	CModel tqmts27("TQMTS27");
	
	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		//对输入信息循环处理
		for (i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++ )
		{   
			//取得单行传入信息
			tqmts27.Reset();
			tqmts27.MergeFrom(bcls_rec->Tables[0].Rows[i]);

			Log::Trace("", "","qmts27_ins IN:---SLAB_NO = [{0}]",(const char*)tqmts27["SLAB_NO"].ToString());
			Log::Trace("", "","qmts27_ins IN:---ISE_TEST_FLAG = [{0}]",tqmts27["ISE_TEST_FLAG"].ToDecimal().ToInt32());

			if (tqmts27["SLAB_NO"].ToString().Trim() == "")
			{
				strcpy(s.msg,_RES("GCRSS0000035")/*材料号不能为空*/);
				throw CApplicationException(-1, s.msg, log.Location);
			}
			//HYF  增加判断，需要输入长度
			if (tqmts27["SAMPLE_LENTH"].ToDecimal() == 0)
			{
				strcpy(s.msg,_RES("MM00S0000001")/*材料长度不能为0。*/);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if(tqmts27["ISE_TEST_FLAG"].ToDecimal() == 3)
			{
				switch(conn->DatabaseKind)
				{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:						// 所有数据库适用，通用SQL语句
						sqlstr = " SELECT COUNT(1) "
								 "   FROM TQMTS27 "
								 "  WHERE SLAB_NO = @slab_no ";
						break;
				}
				cmd_inq.SetCommandText(sqlstr);
				//少了下面这句输入参数的话 HYF 0516
				cmd_inq.Parameters.Set("slab_no", tqmts27["SLAB_NO"].ToString());
				i_count = cmd_inq.ExecuteScalar();
				cmd_inq.Close();
			}
			else if(tqmts27["ISE_TEST_FLAG"].ToDecimal() == 1 || tqmts27["ISE_TEST_FLAG"].ToDecimal() == 2)
			{
				switch(conn->DatabaseKind)
				{
					case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
					case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
					case DB_KIND_MSSQL:				// MS SQL Server数据库
					case DB_KIND_ORACLE:	        // Oracle 数据库
					default:						// 所有数据库适用，通用SQL语句
						sqlstr = " SELECT COUNT(1) "
								 "   FROM TQMTS27 "
								 "  WHERE SLAB_NO = @slab_no "
								 "    AND ISE_TEST_FLAG = @ise_test_flag "
								 /*"     OR ISE_TEST_FLAG = '3') "*/
								 ;
						break;
				}
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.Parameters.Set("slab_no", tqmts27["SLAB_NO"].ToString());
				cmd_inq.Parameters.Set("ise_test_flag", tqmts27["ISE_TEST_FLAG"].ToDecimal());
				i_count = cmd_inq.ExecuteScalar();
				cmd_inq.Close();
			}
			else
			{
				strcpy(s.msg, "检验标记错误！");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if(i_count > 0)
			{
				CFormattable arguments[] = {(const char*)tqmts27["SLAB_NO"].ToString()}; // 定义参数列表的数组
				CMessageFormat::Format(s.msg,_RES("QM00S0003864")/*板坯号[{0}]的低倍硫印实绩已存在，不可重复新增。*/, arguments, 1); //格式化字符串
				throw CApplicationException(-1, s.msg, log.Location);
			}

			/******************** 赋初值 *************************/
			tqmts27["REC_CREATOR"] = s.userid;
			tqmts27["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tqmts27["REC_REVISOR"] = " ";
			tqmts27["REC_REVISE_TIME"] = " ";
			tqmts27["ARCHIVE_FLAG"] = " ";
			tqmts27["AYL_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tqmts27["SAMPLE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
			tqmts27["INSPECT_DEAL_RESP"] = s.userid;

			//HYF0516 查询主档表时，取错了字段，长和宽分别取成了宽和厚
			//问题是27表的长度是4位，主档是5位，因此没法取ROUND(MAT_LEN)
			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = " SELECT HEAT_NO,PONO,ST_NO,PREC_ST_NO,DECI_ST_NO,FIN_ST_NO,JUDGE_ST_NO,ROUND(MAT_LEN),ROUND(MAT_WIDTH) "
							 "   FROM TMMSM01 "
							 "  WHERE MAT_NO = @mat_no ";
					break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("mat_no", tqmts27["SLAB_NO"].ToString());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tqmts27["HEAT_NO"] = cmd_inq.GetString(1);
				tqmts27["PONO"] = cmd_inq.GetString(2);
				tqmts27["ST_NO"] = cmd_inq.GetString(3);
				tqmts27["PREC_ST_NO"] = cmd_inq.GetString(4);
				tqmts27["DECI_ST_NO"] = cmd_inq.GetString(5);
				tqmts27["FIN_ST_NO"] = cmd_inq.GetString(6);
				tqmts27["JUDGE_ST_NO"] = cmd_inq.GetString(7);
				//HYF 0516 字段长度不符，暂时注释，可以人工输入的方式来实现
				//tqmts27["SAMPLE_LENTH"] = cmd_inq.GetDecimal(8);
				tqmts27["SAMPLE_WIDTH"] = cmd_inq.GetDecimal(9);
				if(tqmts27["ST_NO"].ToString().Trim() == "YY000000")
				{
					tqmts27["ST_NO"] = tqmts27["PREC_ST_NO"];
				}
			}
			else
			{
				sprintf(s.msg, "材料[%s]不存在！",(const char*)tqmts27["SLAB_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			cmd_inq.Close();
			Log::Trace("", ""," tqmts27.HEAT_NO = [{0}]",(const char*)tqmts27["HEAT_NO"].ToString());

			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:				// MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
				default:						// 所有数据库适用，通用SQL语句
					sqlstr = " SELECT SM_PLAN_NO "
							 "   FROM TQMTS23 "
							 "  WHERE HEAT_NO = @heat_no ";
					break;
			}
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.Parameters.Set("heat_no", tqmts27["HEAT_NO"].ToString());
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read())
			{
				tqmts27["SM_PLAN_NO"] = cmd_inq.GetString(1);
			}
			cmd_inq.Close();
			Log::Trace("", ""," tqmts27.SM_PLAN_NO = [{0}]",(const char*)tqmts27["SM_PLAN_NO"].ToString());

			//写入低倍硫印实绩
			if(tqmts27["ISE_TEST_FLAG"].ToDecimal() == 1)
			{
				tqmts27["MACRO_CENTER_SEGR"] = 0;
				tqmts27["MACRO_CRACK_INTERNAL"] = 0;
				tqmts27["INCLU"] = 0;
				tqmts27["TRI_CRACK_GRADE"] = 0;
				tqmts27["ANGLE_CRACK_GRADE"] = 0;
				tqmts27["MACULA"] = 0;
				tqmts27["WAFER"] = 0;
				tqmts27["HORE"] = 0;
				tqmts27["NEGSAND_MINUS"] = 0;
				tqmts27.TrimOrBlank();
				tqmts27.Insert();
		    
				//启动低倍硫印判定
				bcls_rec_f.Tables[0].Rows[0]["SLAB_NO"] = tqmts27["SLAB_NO"];				//板坯号
				bcls_rec_f.Tables[0].Rows[0]["ISE_TEST_FLAG"] = tqmts27["ISE_TEST_FLAG"];  //检验标记 1:硫印 2:低倍 3:低倍+硫印

				doFlag = f_qmts27_jud(&bcls_rec_f,&bcls_ret_f,conn);
				if(doFlag != 0)
				{
					Log::Trace("", "","f_qmts27_jud() msg = [{0}]",s.msg);
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			if(tqmts27["ISE_TEST_FLAG"].ToDecimal() == 2)
			{
				tqmts27["CENTER_SGRG_A"] = 0;
				tqmts27["CENTER_SGRG_B"] = 0;
				tqmts27["CENTER_SGRG_C"] = 0;
				tqmts27["CRACK_CENTER"] = 0;
				tqmts27["INTER_CRACK_GRADE"] = 0;
				tqmts27["CLUSTER_1"] = 0;
				tqmts27["CLUSTER_2"] = 0;
				tqmts27.TrimOrBlank();
				tqmts27.Insert();
		    
				//启动低倍硫印判定
				bcls_rec_f.Tables[0].Rows[0]["SLAB_NO"] = tqmts27["SLAB_NO"];				//板坯号
				bcls_rec_f.Tables[0].Rows[0]["ISE_TEST_FLAG"] = tqmts27["ISE_TEST_FLAG"];  //检验标记 1:硫印 2:低倍 3:低倍+硫印

				doFlag = f_qmts27_jud(&bcls_rec_f,&bcls_ret_f,conn);
				if(doFlag != 0)
				{
					Log::Trace("", "","f_qmts27_jud() msg = [{0}]",s.msg);
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
			if(tqmts27["ISE_TEST_FLAG"].ToDecimal() == 3)
			{
				/******************插入硫印信息****************/
				tqmts27["ISE_TEST_FLAG"] = 1;
				CDecimal i_macro_center_segr = tqmts27["MACRO_CENTER_SEGR"];
				CDecimal i_macro_crack_internal = tqmts27["MACRO_CRACK_INTERNAL"];
				CDecimal i_inclu = tqmts27["INCLU"];
				CDecimal i_tri_crack_grade = tqmts27["TRI_CRACK_GRADE"];
				CDecimal i_angle_crack_grade = tqmts27["ANGLE_CRACK_GRADE"];
				CDecimal i_macula = tqmts27["MACULA"];
				CDecimal i_wafer = tqmts27["WAFER"];
				CDecimal i_hore = tqmts27["HORE"];
				CDecimal i_negsand_minus = tqmts27["NEGSAND_MINUS"];

				tqmts27["MACRO_CENTER_SEGR"] = 0;
				tqmts27["MACRO_CRACK_INTERNAL"] = 0;
				tqmts27["INCLU"] = 0;
				tqmts27["TRI_CRACK_GRADE"] = 0;
				tqmts27["ANGLE_CRACK_GRADE"] = 0;
				tqmts27["MACULA"] = 0;
				tqmts27["WAFER"] = 0;
				tqmts27["HORE"] = 0;
				tqmts27["NEGSAND_MINUS"] = 0;

				tqmts27.TrimOrBlank();
				tqmts27.Insert();
		    
				//启动低倍硫印判定
				bcls_rec_f.Tables[0].Rows[0]["SLAB_NO"] = tqmts27["SLAB_NO"];				//板坯号
				bcls_rec_f.Tables[0].Rows[0]["ISE_TEST_FLAG"] = tqmts27["ISE_TEST_FLAG"];  //检验标记 1:硫印 2:低倍 3:低倍+硫印

				doFlag = f_qmts27_jud(&bcls_rec_f,&bcls_ret_f,conn);
				if(doFlag != 0)
				{
					Log::Trace("", "","f_qmts27_jud() msg = [{0}]",s.msg);
					throw CApplicationException(-1, s.msg, log.Location);
				}

				/******************插入低倍信息****************/
				tqmts27["ISE_TEST_FLAG"] = 2;
				tqmts27["CENTER_SGRG_A"] = 0;
				tqmts27["CENTER_SGRG_B"] = 0;
				tqmts27["CENTER_SGRG_C"] = 0;
				tqmts27["CRACK_CENTER"] = 0;
				tqmts27["INTER_CRACK_GRADE"] = 0;
				tqmts27["CLUSTER_1"] = 0;
				tqmts27["CLUSTER_2"] = 0;

				tqmts27["MACRO_CENTER_SEGR"] = i_macro_center_segr;
				tqmts27["MACRO_CRACK_INTERNAL"] = i_macro_crack_internal;
				tqmts27["INCLU"] = i_inclu;
				tqmts27["TRI_CRACK_GRADE"] = i_tri_crack_grade;
				tqmts27["ANGLE_CRACK_GRADE"] = i_angle_crack_grade;
				tqmts27["MACULA"] = i_macula;
				tqmts27["WAFER"] = i_wafer;
				tqmts27["HORE"] = i_hore;
				tqmts27["NEGSAND_MINUS"] = i_negsand_minus;

				tqmts27.TrimOrBlank();
				tqmts27.Insert();
		    
				//启动低倍硫印判定
				bcls_rec_f.Tables[0].Rows[0]["SLAB_NO"] = tqmts27["SLAB_NO"];				//板坯号
				bcls_rec_f.Tables[0].Rows[0]["ISE_TEST_FLAG"] = tqmts27["ISE_TEST_FLAG"];  //检验标记 1:硫印 2:低倍 3:低倍+硫印

				doFlag = f_qmts27_jud(&bcls_rec_f,&bcls_ret_f,conn);
				if(doFlag != 0)
				{
					Log::Trace("", "","f_qmts27_jud() msg = [{0}]",s.msg);
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
		}
		//sprintf(s.msg,_RES("GCRSS0000018")/*新增信息失败。*/);
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
