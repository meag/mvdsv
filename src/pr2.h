/*
 *  QW262
 *  Copyright (C) 2004  [sd] angel
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 */

#ifndef __PR2_H__
#define __PR2_H__


extern intptr_t sv_syscall(intptr_t arg, ...);
extern int sv_sys_callex(byte *data, unsigned int len, int fn, pr2val_t*arg);
typedef void (*pr2_trapcall_t)(byte* base, uintptr_t mask, pr2val_t* stack, pr2val_t*retval);

//extern int usedll;
extern cvar_t sv_progtype;
extern vm_t* sv_vm;


void		PR2_Init();
#define PR_Init PR2_Init
void		PR2_UnLoadProgs();
#define PR_UnLoadProgs PR2_UnLoadProgs
void		PR2_LoadProgs();
#define PR_LoadProgs PR2_LoadProgs
void		PR2_GameStartFrame();
#define PR_GameStartFrame PR2_GameStartFrame
void		PR2_LoadEnts(char *data);
#define PR_LoadEnts PR2_LoadEnts
void		PR2_GameClientConnect(int spec);
#define PR_GameClientConnect PR2_GameClientConnect
void		PR2_GamePutClientInServer(int spec);
#define PR_GamePutClientInServer PR2_GamePutClientInServer
void		PR2_GameClientDisconnect(int spec);
#define PR_GameClientDisconnect PR2_GameClientDisconnect
void		PR2_GameClientPreThink(int spec);
#define PR_GameClientPreThink PR2_GameClientPreThink
void		PR2_GameClientPostThink(int spec);
#define PR_GameClientPostThink PR2_GameClientPostThink
qbool		PR2_ClientCmd();
#define PR_ClientCmd PR2_ClientCmd
void		PR2_ClientKill();
#define PR_ClientKill PR2_ClientKill
qbool		PR2_ClientSay(int isTeamSay, char *message);
#define PR_ClientSay PR2_ClientSay
void		PR2_GameSetNewParms();
#define PR_GameSetNewParms PR2_GameSetNewParms
void		PR2_GameSetChangeParms();
#define PR_GameSetChangeParms PR2_GameSetChangeParms
void		PR2_EdictTouch(func_t f);
#define PR_EdictTouch PR2_EdictTouch
void		PR2_EdictThink(func_t f);
#define PR_EdictThink PR2_EdictThink
void		PR2_EdictBlocked(func_t f);
#define PR_EdictBlocked PR2_EdictBlocked
qbool 		PR2_UserInfoChanged();
#define PR_UserInfoChanged PR2_UserInfoChanged
void 		PR2_GameShutDown();
#define PR_GameShutDown PR2_GameShutDown
void 		PR2_GameConsoleCommand(void);
void		PR2_PausedTic(float duration);
#define PR_PausedTic PR2_PausedTic

char*		PR2_GetString(intptr_t);
#define		PR_GetString PR2_GetString
intptr_t	PR2_SetString(char*s);
#define PR_SetString PR2_SetString
void		PR2_RunError(char *error, ...);
eval_t*		PR2_GetEdictFieldValue(edict_t *ed, char *field);
#define PR_GetEdictFieldValue PR2_GetEdictFieldValue
int			ED2_FindFieldOffset(char *field);
#define ED_FindFieldOffset ED2_FindFieldOffset
void 		PR2_InitProg();
#define PR_InitProg PR2_InitProg

// Getters & setters for PR2 - globals
#ifndef PR2_ONLY
void       PR2_SetStringByPointer(string_t* pr1, pr2_string_t* pr2, char* stringValue);
void       PR2_SetIntByPointer(int* pr1, int* pr2, int intValue);
void       PR2_SetFloatByPointer(float* pr1, float* pr2, float floatValue);
void       PR2_SetVectorByPointer(vec3_t* pr1, vec3_t* pr2, vec3_t vectorValue);
void       PR2_SetFuncByPointer(func_t* pr1, pr2_func_t* pr2, pr2_func_t funcValue);

char*      PR2_GetStringByPointer(string_t* pr1, pr2_string_t* pr2);
int        PR2_GetIntByPointer(int* pr1, int* pr2);
float      PR2_GetFloatByPointer(float* pr1, float* pr2);
float*     PR2_GetVectorByPointer(vec3_t* pr1, vec3_t* pr2);
pr2_func_t PR2_GetFuncByPointer(func_t* pr1, pr2_func_t* pr2);

#define PR_SetGlobalString(globalname,val) PR2_SetStringByPointer(&pr_global_struct->globalname, &((pr2_globalvars_t*)pr_global_struct)->globalname, val)
#define PR_SetGlobalInt(globalname,val) PR2_SetIntByPointer(&pr_global_struct->globalname, &((pr2_globalvars_t*)pr_global_struct)->globalname, val)
#define PR_SetGlobalFloat(globalname,val) PR2_SetFloatByPointer(&pr_global_struct->globalname, &((pr2_globalvars_t*)pr_global_struct)->globalname, val)
#define PR_SetGlobalVector(globalname,val) PR2_SetVectorByPointer(&pr_global_struct->globalname, &((pr2_globalvars_t*)pr_global_struct)->globalname, val)
#define PR_SetGlobalFunc(globalname,val) PR2_SetFuncByPointer(&pr_global_struct->globalname, &((pr2_globalvars_t*)pr_global_struct)->globalname, val)

#define PR_GetGlobalString(globalname) PR2_GetStringByPointer(&pr_global_struct->globalname, &((pr2_globalvars_t*)pr_global_struct)->globalname)
#define PR_GetGlobalInt(globalname) PR2_GetIntByPointer(&pr_global_struct->globalname, &((pr2_globalvars_t*)pr_global_struct)->globalname)
#define PR_GetGlobalFloat(globalname) PR2_GetFloatByPointer(&pr_global_struct->globalname, &((pr2_globalvars_t*)pr_global_struct)->globalname)
#define PR_GetGlobalVector(globalname) PR2_GetVectorByPointer(&pr_global_struct->globalname, &((pr2_globalvars_t*)pr_global_struct)->globalname)
#define PR_GetGlobalFunc(globalname) PR2_GetFuncByPointer(&pr_global_struct->globalname, &((pr2_globalvars_t*)pr_global_struct)->globalname)

// Getters & setters for PR2 - Entity fields
#define PR_SetEntityString(ent,fieldname,val) PR2_SetStringByPointer(&ent->v.fieldname, &(*(pr2_entvars_t*)&ent->v).fieldname, val)
#define PR_SetEntityInt(ent,fieldname,val) PR2_SetIntByPointer(&ent->v.fieldname, &(*(pr2_entvars_t*)&ent->v).fieldname, val)
#define PR_SetEntityFloat(ent,fieldname,val) PR2_SetFloatByPointer(&ent->v.fieldname, &(*(pr2_entvars_t*)&ent->v).fieldname, val)
#define PR_SetEntityVector(ent,fieldname,val) PR2_SetVectorByPointer(&ent->v.fieldname, &(*(pr2_entvars_t*)&ent->v).fieldname, val)
#define PR_SetEntityFunc(ent,fieldname,val) PR2_SetFuncByPointer(&ent->v.fieldname, &(*(pr2_entvars_t*)&ent->v).fieldname, val)

#define PR_GetEntityString(ent,fieldname) PR2_GetStringByPointer(&ent->v.fieldname, &(*(pr2_entvars_t*)&ent->v).fieldname)
#define PR_GetEntityInt(ent,fieldname) PR2_GetIntByPointer(&ent->v.fieldname, &(*(pr2_entvars_t*)&ent->v).fieldname)
#define PR_GetEntityFloat(ent,fieldname) PR2_GetFloatByPointer(&ent->v.fieldname, &(*(pr2_entvars_t*)&ent->v).fieldname)
#define PR_GetEntityVector(ent,fieldname) PR2_GetVectorByPointer(&ent->v.fieldname, &(*(pr2_entvars_t*)&ent->v).fieldname)
#define PR_GetEntityFunc(ent,fieldname) PR2_GetFuncByPointer(&ent->v.fieldname, &(*(pr2_entvars_t*)&ent->v).fieldname)
#else
#define PR_SetGlobalString(globalname,val) ((pr2_globalvars_t*)pr_global_struct)->globalname = PR2_SetString(val)
#define PR_SetGlobalInt(globalname,val) ((pr2_globalvars_t*)pr_global_struct)->globalname = val
#define PR_SetGlobalFloat(globalname,val) ((pr2_globalvars_t*)pr_global_struct)->globalname = val
#define PR_SetGlobalVector(globalname,val) VectorCopy(val, ((pr2_globalvars_t*)pr_global_struct)->globalname)
#define PR_SetGlobalFunc(globalname,val) ((pr2_globalvars_t*)pr_global_struct)->globalname = val

#define PR_GetGlobalString(globalname) PR2_GetString(((pr2_globalvars_t*)pr_global_struct)->globalname)
#define PR_GetGlobalInt(globalname) ((pr2_globalvars_t*)pr_global_struct)->globalname
#define PR_GetGlobalFloat(globalname) ((pr2_globalvars_t*)pr_global_struct)->globalname
#define PR_GetGlobalVector(globalname) ((pr2_globalvars_t*)pr_global_struct)->globalname
#define PR_GetGlobalFunc(globalname) ((pr2_globalvars_t*)pr_global_struct)->globalname

// Getters & setters for PR2 - Entity fields
#define PR_SetEntityString(ent,fieldname,val) (*(pr2_entvars_t*)&ent->v).fieldname = PR2_SetString(val)
#define PR_SetEntityInt(ent,fieldname,val) (*(pr2_entvars_t*)&ent->v).fieldname = val
#define PR_SetEntityFloat(ent,fieldname,val) (*(pr2_entvars_t*)&ent->v).fieldname = val
#define PR_SetEntityVector(ent,fieldname,val) VectorCopy(val, (*(pr2_entvars_t*)&ent->v).fieldname)
#define PR_SetEntityFunc(ent,fieldname,val) (*(pr2_entvars_t*)&ent->v).fieldname = val

#define PR_GetEntityString(ent,fieldname) PR2_GetString((*(pr2_entvars_t*)&ent->v).fieldname)
#define PR_GetEntityInt(ent,fieldname) (*(pr2_entvars_t*)&ent->v).fieldname
#define PR_GetEntityFloat(ent,fieldname) (*(pr2_entvars_t*)&ent->v).fieldname
#define PR_GetEntityVector(ent,fieldname) (*(pr2_entvars_t*)&ent->v).fieldname
#define PR_GetEntityFunc(ent,fieldname) (*(pr2_entvars_t*)&ent->v).fieldname
#endif

typedef struct
{	int	    pad[28];
	int	    self;
	int	    other;
	int	    world;
	float	time;
	float	frametime;
	int	    newmis;
	float	force_retouch;
	pr2_string_t mapname;
	float	serverflags;
	float	total_secrets;
	float	total_monsters;
	float	found_secrets;
	float	killed_monsters;
	float	parm1;
	float	parm2;
	float	parm3;
	float	parm4;
	float	parm5;
	float	parm6;
	float	parm7;
	float	parm8;
	float	parm9;
	float	parm10;
	float	parm11;
	float	parm12;
	float	parm13;
	float	parm14;
	float	parm15;
	float	parm16;
	vec3_t	v_forward;
	vec3_t	v_up;
	vec3_t	v_right;
	float	trace_allsolid;
	float	trace_startsolid;
	float	trace_fraction;
	vec3_t	trace_endpos;
	vec3_t	trace_plane_normal;
	float	trace_plane_dist;
	int	    trace_ent;
	float	trace_inopen;
	float	trace_inwater;
	int	    msg_entity;
	pr2_func_t	main;
	pr2_func_t	StartFrame;
	pr2_func_t	PlayerPreThink;
	pr2_func_t	PlayerPostThink;
	pr2_func_t	ClientKill;
	pr2_func_t	ClientConnect;
	pr2_func_t	PutClientInServer;
	pr2_func_t	ClientDisconnect;
	pr2_func_t	SetNewParms;
	pr2_func_t	SetChangeParms;
} pr2_globalvars_t;

typedef struct
{
	float	modelindex;
	vec3_t	absmin;
	vec3_t	absmax;
	float	ltime;
	float	lastruntime;
	float	movetype;
	float	solid;
	vec3_t	origin;
	vec3_t	oldorigin;
	vec3_t	velocity;
	vec3_t	angles;
	vec3_t	avelocity;
	pr2_string_t	classname;
	pr2_string_t	model;
	float	frame;
	float	skin;
	float	effects;
	vec3_t	mins;
	vec3_t	maxs;
	vec3_t	size;
	pr2_func_t	touch;
	pr2_func_t	use;
	pr2_func_t	think;
	pr2_func_t	blocked;
	float	nextthink;
	int	groundentity;
	float	health;
	float	frags;
	float	weapon;
	pr2_string_t	weaponmodel;
	float	weaponframe;
	float	currentammo;
	float	ammo_shells;
	float	ammo_nails;
	float	ammo_rockets;
	float	ammo_cells;
	float	items;
	float	takedamage;
	int	chain;
	float	deadflag;
	vec3_t	view_ofs;
	float	button0;
	float	button1;
	float	button2;
	float	impulse;
	float	fixangle;
	vec3_t	v_angle;
	pr2_string_t	netname;
	int	enemy;
	float	flags;
	float	colormap;
	float	team;
	float	max_health;
	float	teleport_time;
	float	armortype;
	float	armorvalue;
	float	waterlevel;
	float	watertype;
	float	ideal_yaw;
	float	yaw_speed;
	int	aiment;
	int	goalentity;
	float	spawnflags;
	pr2_string_t	target;
	pr2_string_t	targetname;
	float	dmg_take;
	float	dmg_save;
	int	dmg_inflictor;
	int	owner;
	vec3_t	movedir;
	pr2_string_t	message;
	float	sounds;
	pr2_string_t	noise;
	pr2_string_t	noise1;
	pr2_string_t	noise2;
	pr2_string_t	noise3;
} pr2_entvars_t;

#endif /* !__PR2_H__ */
