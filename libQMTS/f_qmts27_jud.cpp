/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2010
Author:      冯晓轶
Version:     1.0
Date:        2012-04-10
Description: 判定低倍硫印
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件，请包含在""中



/******后台pc文件标准注释标记*****/
/*<remark>=========================================================
/// <summary>
///判定低倍硫印
/// <para>
/// 1.判定低倍硫印。
/// 
/// </para>
/// <para>数据库表：TQMTS27(实绩表)
///							TQMTS0X(工艺卡)  
/// <para>主调用函数：qmts27_ins()/qmts27_upd()调用		</para>
/// <para>需调用函数： f_qmts_pz99      //炉次判定模型  </para>
///                    f_mmsmsm03       //炼钢侧		</para>
///                    f_mmhpsm99       //轧钢侧		</para>
/// </summary>
/// <param name="SLAB_NO">板坯号             </param>
/// <returns>  </returns>
===========================================================</remark>*/


/* ***** 外部函数申明 ***** */
//
//BM2_FUNCTION_IMPORT
 //int f_qmts_pz99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn);//启动炉次判定模型
//
//BM2_FUNCTION_IMPORT
 //int f_mmsm_qm03(EIClass *bcls_rec,EIClass *bcls_ret, CDbConnection *conn);  //(炼钢侧)板坯材料修改性能判定代码
//
//BM2_FUNCTION_IMPORT
// int f_mmhpsm99(EIClass *bcls_rec,EIClass *bcls_ret, CDbConnection *conn);  //(轧钢侧)


BM2_FUNCTION_EXPORT
 int f_qmts27_jud(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection *conn)
{
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义

	//程序用变量
	int doFlag = 0;
	int i = 0;
	int fetchRowCount = 0;

	int v_judge_code[17];//各项判定结果

	CString sqlstr = "";

	/* 实体类定义 */
	CModel tqmts27("TQMTS27");
	CModel tqmts0x("TQMTS0X");

	/* 数据库操作类定义：统一放在Service或函数前段 */
	CDbCommand cmd_inq(conn);

	try
	{
		//获得输入参数
		tqmts27.Reset();
		tqmts27.MergeFrom(bcls_rec->Tables[0].Rows[0]);

		Log::Trace("", __FUNCTION__, "f_qmts27_jud IN:---SLAB_NO = [{0}]",(const char*)tqmts27["SLAB_NO"].ToString());
		Log::Trace("", __FUNCTION__, "f_qmts27_jud IN:---ISE_TEST_FLAG = [{0}]",tqmts27["ISE_TEST_FLAG"].ToDecimal().ToInt32());

		//查低倍硫印表，获取各项低倍硫印实绩数据/
		switch(conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT * "
				"   FROM TQMTS27 "
				"  WHERE SLAB_NO = @slab_no "
				"    AND ISE_TEST_FLAG = @ise_test_flag ";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);		
		cmd_inq.Parameters.Set("slab_no", tqmts27["SLAB_NO"].ToString());
		cmd_inq.Parameters.Set("ise_test_flag", tqmts27["ISE_TEST_FLAG"].ToDecimal());
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			cmd_inq.Fetch(tqmts27);
		}
		cmd_inq.Close();

		//查工艺卡表，获取各项低倍硫印标准数据
		switch(conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:						// 所有数据库适用，通用SQL语句
			sqlstr = " SELECT * "
				"   FROM TQMTS0X "
				"  WHERE ST_NO = @st_no ";
			break;
		}
		cmd_inq.SetCommandText(sqlstr);		
		cmd_inq.Parameters.Set("st_no", tqmts27["ST_NO"].ToString());
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			cmd_inq.Fetch(tqmts0x);
		}
		cmd_inq.Close();

		//判定中文说明的初始化
		tqmts27["ISE_REMARK"] = " ";

		//判定偏析A
		if(tqmts0x["CENTER_SGRG_A"].ToDecimal() == 0 || tqmts0x["CENTER_SGRG_A"].ToDecimal() == 99)
		{
			v_judge_code[1] = 1;
		}
		else
		{
			if(tqmts27["CENTER_SGRG_A"].ToDecimal() > tqmts0x["CENTER_SGRG_A"].ToDecimal())
			{
				v_judge_code[1] = 0;
			}
			else
			{
				v_judge_code[1] = 1;
			}
		}
		Log::Trace("", __FUNCTION__, "偏析A [{0}]",v_judge_code[1]); 

		//判定偏析B
		if(tqmts0x["CENTER_SGRG_B"].ToDecimal() == 0 || tqmts0x["CENTER_SGRG_B"].ToDecimal() == 99)
		{
			v_judge_code[2] = 1;
		}
		else
		{
			if(tqmts27["CENTER_SGRG_B"].ToDecimal() > tqmts0x["CENTER_SGRG_B"].ToDecimal())
			{
				v_judge_code[2] = 0;
			}
			else
			{
				v_judge_code[2] = 1;
			}
		}
		Log::Trace("", __FUNCTION__, "偏析B [{0}]",v_judge_code[2]);

		//判定偏析C
		if(tqmts0x["CENTER_SGRG_C"].ToDecimal() == 0 || tqmts0x["CENTER_SGRG_C"].ToDecimal() == 99)
		{
			v_judge_code[3] = 1;
		}
		else
		{
			if(tqmts27["CENTER_SGRG_C"].ToDecimal() > tqmts0x["CENTER_SGRG_C"].ToDecimal())
			{
				v_judge_code[3] = 0;
			}
			else
			{
				v_judge_code[3] = 1;
			}
		}
		Log::Trace("", __FUNCTION__, "偏析C [{0}]",v_judge_code[3]);

		//判定偏析
		if(tqmts0x["MACRO_CENTER_SEGR"].ToDecimal() == 0 || tqmts0x["MACRO_CENTER_SEGR"].ToDecimal() == 99)
		{
			v_judge_code[4] = 1;
		}
		else
		{
			if(tqmts27["MACRO_CENTER_SEGR"].ToDecimal() > tqmts0x["MACRO_CENTER_SEGR"].ToDecimal())
			{
				v_judge_code[4] = 0;
			}
			else
			{
				v_judge_code[4] = 1;
			}
		}
		Log::Trace("", __FUNCTION__, "偏析 [{0}]",v_judge_code[4]);

		v_judge_code[0] = 1;
		if(tqmts27["ISE_TEST_FLAG"].ToDecimal() == 1)
		{
			v_judge_code[4] = 1;
		}
		if(tqmts27["ISE_TEST_FLAG"].ToDecimal() == 2)
		{
			v_judge_code[1] = 1;
			v_judge_code[2] = 1;
			v_judge_code[3] = 1;
		}

		for(i=1;i<=4;i++)	 
		{		
			Log::Trace("", __FUNCTION__, " i=[{0}]:[{1}]",i,v_judge_code[i]);
			v_judge_code[0] = v_judge_code[0] * v_judge_code[i];
		}

		if(v_judge_code[0] == 1)
		{
			tqmts27["SEGR_FLAG"] = "1"; //合格
		}
		else
		{
			tqmts27["SEGR_FLAG"] = "0";  // 不合格
			tqmts27["ISE_REMARK"] = tqmts27["ISE_REMARK"].ToString() + "偏析不合格，";
		} 
		Log::Trace("", __FUNCTION__, "总偏析 [{0}]",v_judge_code[0]);

		//判定中裂
		tqmts27["CRACK_CENTER_FLAG"] = "1";
		v_judge_code[5] = 1;
		if(tqmts27["ISE_TEST_FLAG"].ToDecimal() != 2)
		{
			if(tqmts0x["CRACK_CENTER"].ToDecimal() == 0 || tqmts0x["CRACK_CENTER"].ToDecimal() == 99)
			{
				v_judge_code[5] = 1;
			}
			else
			{
				if(tqmts27["CRACK_CENTER"].ToDecimal() > tqmts0x["CRACK_CENTER"].ToDecimal())
				{
					v_judge_code[5] = 0;
					tqmts27["CRACK_CENTER_FLAG"] = "0";
					tqmts27["ISE_REMARK"] = tqmts27["ISE_REMARK"].ToString() + "中裂不合格，";
				}
				else
				{
					v_judge_code[5] = 1;
				}
			}
		}
		Log::Trace("", __FUNCTION__, "中裂 [{0}]",v_judge_code[5]);

		//判定内裂
		if(tqmts0x["INTER_CRACK_GRADE"].ToDecimal() == 0 || tqmts0x["INTER_CRACK_GRADE"].ToDecimal() == 99)
		{
			v_judge_code[6] = 1;
		}
		else
		{
			if(tqmts27["INTER_CRACK_GRADE"].ToDecimal() > tqmts0x["INTER_CRACK_GRADE"].ToDecimal())
			{
				v_judge_code[6] = 0;
			}
			else
			{
				v_judge_code[6] = 1;
			}
		}
		Log::Trace("", __FUNCTION__, "内裂 [{0}]",v_judge_code[6]);

		//判定低倍内裂
		if(tqmts0x["MACRO_CRACK_INTERNAL"].ToDecimal() == 0 || tqmts0x["MACRO_CRACK_INTERNAL"].ToDecimal() == 99)
		{
			v_judge_code[7] = 1;
		}
		else
		{
			if(tqmts27["MACRO_CRACK_INTERNAL"].ToDecimal() > tqmts0x["MACRO_CRACK_INTERNAL"].ToDecimal())
			{
				v_judge_code[7] = 0;
			}
			else
			{
				v_judge_code[7] = 1;
			}
		}
		Log::Trace("", __FUNCTION__, "低倍内裂 [{0}]",v_judge_code[7]);

		v_judge_code[0] = 1;
		if(tqmts27["ISE_TEST_FLAG"].ToDecimal() == 1)
		{
			v_judge_code[7] = 1;
		}
		if(tqmts27["ISE_TEST_FLAG"].ToDecimal() == 2)
		{
			v_judge_code[6] = 1;
		}

		for(i=6;i<=7;i++)	 
		{		
			Log::Trace("", __FUNCTION__, " i=[{0}]:[{1}]",i,v_judge_code[i]);
			v_judge_code[0] = v_judge_code[0] * v_judge_code[i];
		}

		if(v_judge_code[0] == 1)
		{
			tqmts27["INSIDE_FLAG"] = "1"; //合格
		}
		else
		{
			tqmts27["INSIDE_FLAG"] = "0";  // 不合格
			tqmts27["ISE_REMARK"] = tqmts27["ISE_REMARK"].ToString() + "内裂不合格，";
		} 
		Log::Trace("", __FUNCTION__, "总内裂 [{0}]",v_judge_code[0]);

		//判定夹杂1
		if(tqmts0x["CLUSTER_1"].ToDecimal() == 0 || tqmts0x["CLUSTER_1"].ToDecimal() == 99)
		{
			v_judge_code[8] = 1;
		}
		else
		{
			if(tqmts27["CLUSTER_1"].ToDecimal() > tqmts0x["CLUSTER_1"].ToDecimal())
			{
				v_judge_code[8] = 0;
			}
			else
			{
				v_judge_code[8] = 1;
			}
		}
		Log::Trace("", __FUNCTION__, "夹杂1 [{0}]",v_judge_code[8]);

		//判定夹杂2
		if(tqmts0x["CLUSTER_2"].ToDecimal() == 0 || tqmts0x["CLUSTER_2"].ToDecimal() == 99)
		{
			v_judge_code[9] = 1;
		}
		else
		{
			if(tqmts27["CLUSTER_2"].ToDecimal() > tqmts0x["CLUSTER_2"].ToDecimal())
			{
				v_judge_code[9] = 0;
			}
			else
			{
				v_judge_code[9] = 1;
			}
		}
		Log::Trace("", __FUNCTION__, "夹杂2 [{0}]",v_judge_code[9]);

		//判定夹杂
		if(tqmts0x["INCLU"].ToDecimal() == 0 || tqmts0x["INCLU"].ToDecimal() == 99)
		{
			v_judge_code[10] = 1;
		}
		else
		{
			if(tqmts27["INCLU"].ToDecimal() > tqmts0x["INCLU"].ToDecimal())
			{
				v_judge_code[10] = 0;
			}
			else
			{
				v_judge_code[10] = 1;
			}
		}
		Log::Trace("", __FUNCTION__, "夹杂 [{0}]",v_judge_code[10]);

		v_judge_code[0] = 1;
		if(tqmts27["ISE_TEST_FLAG"].ToDecimal() == 1)
		{
			v_judge_code[10] = 1;
		}
		if(tqmts27["ISE_TEST_FLAG"].ToDecimal() == 2)
		{
			v_judge_code[8] = 1;
			v_judge_code[9] = 1;
		}

		for(i=8;i<=10;i++)	 
		{		
			Log::Trace("", __FUNCTION__, " i=[{0}]:[{1}]",i,v_judge_code[i]);
			v_judge_code[0] = v_judge_code[0] * v_judge_code[i];
		}

		if(v_judge_code[0] == 1)
		{
			tqmts27["AL2O3_FLAG"] = "1"; //合格
		}
		else
		{
			tqmts27["AL2O3_FLAG"] = "0";  // 不合格
			tqmts27["ISE_REMARK"] = tqmts27["ISE_REMARK"].ToString() + "夹杂不合格，";
		} 
		Log::Trace("", __FUNCTION__, "总夹杂 [{0}]",v_judge_code[0]);

		//判定三角区裂纹
		tqmts27["TRI_CRACK_FLAG"] = "1";
		v_judge_code[11] = 1;
		if(tqmts27["ISE_TEST_FLAG"].ToDecimal() != 1)
		{
			if(tqmts0x["TRI_CRACK_GRADE"].ToDecimal() == 0 || tqmts0x["TRI_CRACK_GRADE"].ToDecimal() == 99)
			{
				v_judge_code[11] = 1;
			}
			else
			{
				if(tqmts27["TRI_CRACK_GRADE"].ToDecimal() > tqmts0x["TRI_CRACK_GRADE"].ToDecimal())
				{
					v_judge_code[11] = 0;
					tqmts27["TRI_CRACK_FLAG"] = "0";
					tqmts27["ISE_REMARK"] = tqmts27["ISE_REMARK"].ToString() + "三角区裂纹不合格，";
				}
				else
				{
					v_judge_code[11] = 1;
				}
			}
		}
		Log::Trace("", __FUNCTION__, "三角区裂纹 [{0}]",v_judge_code[11]);

		//判定角裂
		tqmts27["HORN_FLAG"] = "1";
		v_judge_code[12] = 1;
		if(tqmts27["ISE_TEST_FLAG"].ToDecimal() != 1)
		{
			if(tqmts0x["ANGLE_CRACK_GRADE"].ToDecimal() == 0 || tqmts0x["ANGLE_CRACK_GRADE"].ToDecimal() == 99)
			{
				v_judge_code[12] = 1;
			}
			else
			{
				if(tqmts27["ANGLE_CRACK_GRADE"].ToDecimal() > tqmts0x["ANGLE_CRACK_GRADE"].ToDecimal())
				{
					v_judge_code[12] = 0;
					tqmts27["HORN_FLAG"] = "0";
					tqmts27["ISE_REMARK"] = tqmts27["ISE_REMARK"].ToString() + "角裂不合格，";
				}
				else
				{
					v_judge_code[12] = 1;
				}
			}
		}
		Log::Trace("", __FUNCTION__, "角裂 [{0}]",v_judge_code[12]);

		//判定黑点
		tqmts27["MACULA_FLAG"] = "1";
		v_judge_code[13] = 1;
		if(tqmts27["ISE_TEST_FLAG"].ToDecimal() != 1)
		{
			if(tqmts0x["MACULA"].ToDecimal() == 0 || tqmts0x["MACULA"].ToDecimal() == 99)
			{
				v_judge_code[13] = 1;
			}
			else
			{
				if(tqmts27["MACULA"].ToDecimal() > tqmts0x["MACULA"].ToDecimal())
				{
					v_judge_code[13] = 0;
					tqmts27["MACULA_FLAG"] = "0";
					tqmts27["ISE_REMARK"] = tqmts27["ISE_REMARK"].ToString() + "黑点不合格，";
				}
				else
				{
					v_judge_code[13] = 1;
				}
			}
		}
		Log::Trace("", __FUNCTION__, "黑点 [{0}]",v_judge_code[13]);

		//判定等轴晶率
		tqmts27["WAFER_FLAG"] = "1";
		v_judge_code[14] = 1;
		if(tqmts27["ISE_TEST_FLAG"].ToDecimal() != 1)
		{
			if(tqmts0x["WAFER"].ToDecimal() == 0 || tqmts0x["WAFER"].ToDecimal() == 99)
			{
				v_judge_code[14] = 1;
			}
			else
			{
				if(tqmts27["WAFER"].ToDecimal() > tqmts0x["WAFER"].ToDecimal())
				{
					v_judge_code[14] = 0;
					tqmts27["WAFER_FLAG"] = "0";
					tqmts27["ISE_REMARK"] = tqmts27["ISE_REMARK"].ToString() + "等轴晶率不合格，";
				}
				else
				{
					v_judge_code[14] = 1;
				}
			}
		}
		Log::Trace("", __FUNCTION__, "等轴晶率 [{0}]",v_judge_code[14]);

		//判定缩孔
		tqmts27["HORE_FLAG"] = "1";
		v_judge_code[15] = 1;
		if(tqmts27["ISE_TEST_FLAG"].ToDecimal() != 1)
		{
			if(tqmts0x["HORE"].ToDecimal() == 0 || tqmts0x["HORE"].ToDecimal() == 99)
			{
				v_judge_code[15] = 1;
			}
			else
			{
				if(tqmts27["HORE"].ToDecimal() > tqmts0x["HORE"].ToDecimal())
				{
					v_judge_code[15] = 0;
					tqmts27["HORE_FLAG"] = "0";
					tqmts27["ISE_REMARK"] = tqmts27["ISE_REMARK"].ToString() + "缩孔不合格，";
				}
				else
				{
					v_judge_code[15] = 1;
				}
			}
		}
		Log::Trace("", __FUNCTION__, "缩孔 [{0}]",v_judge_code[15]);

		//判定负偏析
		tqmts27["NEGSAND_MINUS_FLAG"] = "1";
		v_judge_code[16] = 1;
		if(tqmts27["ISE_TEST_FLAG"].ToDecimal() != 1)
		{
			if(tqmts0x["NEGSAND_MINUS"].ToDecimal() == 0 || tqmts0x["NEGSAND_MINUS"].ToDecimal() == 99)
			{
				v_judge_code[16] = 1;
			}
			else
			{
				if(tqmts27["NEGSAND_MINUS"].ToDecimal() > tqmts0x["NEGSAND_MINUS"].ToDecimal())
				{
					v_judge_code[16] = 0;
					tqmts27["NEGSAND_MINUS_FLAG"] = "0";
					tqmts27["ISE_REMARK"] = tqmts27["ISE_REMARK"].ToString() + "负偏析不合格，";
				}
				else
				{
					v_judge_code[16] = 1;
				}
			}
		}
		Log::Trace("", __FUNCTION__, "负偏析 [{0}]",v_judge_code[16]);

		v_judge_code[0] = 1;
		for (i = 1; i <= 16; i++)
		{
			v_judge_code[0] = v_judge_code[0] * v_judge_code[i];
		}
		Log::Trace("", __FUNCTION__, "v_judge_code[{0}]",v_judge_code[0]);  

		if(v_judge_code[0] == 1)
		{
			tqmts27["JUDGE_CODE"] = "1"; //合格
		}
		else
		{
			tqmts27["JUDGE_CODE"] = "0"; //不合格
		} 

		Log::Trace("", __FUNCTION__, "更新表tqmts27之前的判定结果中文备注：ISE_REMARK[{0}]", tqmts27["ISE_REMARK"].ToString());

		tqmts27.Update("SEGR_FLAG,CRACK_CENTER_FLAG,INSIDE_FLAG,AL2O3_FLAG,TRI_CRACK_FLAG,HORN_FLAG,MACULA_FLAG,WAFER_FLAG,HORE_FLAG,NEGSAND_MINUS_FLAG,JUDGE_CODE,ISE_REMARK",  //修改字段项
			"SLAB_NO,ISE_TEST_FLAG"); //条件字段项

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
		Log::Trace("", __FUNCTION__, "tqmts27.ISE_FLAG[{0}]",tqmts27["ISE_FLAG"].ToDecimal().ToInt32());  

		tqmts27.Update("ISE_FLAG",  /*修改字段项*/
			"SLAB_NO"); /*条件字段项*/
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

