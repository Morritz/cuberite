
// Protocol_1_11.cpp

/*
Implements the 1.11 protocol classes:
	- cProtocol_1_11_0
		- release 1.11 protocol (#315)
	- cProtocol_1_11_1
		- release 1.11.1 protocol (#316)
*/

#include "Globals.h"
#include "Protocol_1_11.h"
#include "Packetizer.h"

#include "../WorldStorage/FastNBT.h"

#include "../Entities/Boat.h"
#include "../Entities/EnderCrystal.h"
#include "../Entities/ExpOrb.h"
#include "../Entities/Minecart.h"
#include "../Entities/FallingBlock.h"
#include "../Entities/Painting.h"
#include "../Entities/Pickup.h"
#include "../Entities/Player.h"
#include "../Entities/ItemFrame.h"
#include "../Entities/ArrowEntity.h"
#include "../Entities/FireworkEntity.h"
#include "../Entities/SplashPotionEntity.h"

#include "../Mobs/IncludeAllMonsters.h"

#include "../BlockEntities/BedEntity.h"
#include "../BlockEntities/MobSpawnerEntity.h"

#include "../Root.h"
#include "../Server.h"
#include "../ClientHandle.h"
#include "../CompositeChat.h"
#include "../JsonUtils.h"
#include "../Bindings/PluginManager.h"





// The disabled error is intended, since the Metadata have overlapping indexes
// based on the type of the Entity.
//
// IMPORTANT: The enum is used to automate the sequential counting of the
// Metadata indexes. Adding a new enum value causes the following values to
// increase their index. Therefore the ordering of the enum values is VERY important!
#ifdef __clang__
	#pragma clang diagnostic push
	#pragma clang diagnostic ignored "-Wduplicate-enum"
#endif

namespace Metadata_1_11
{
	enum MetadataIndex
	{
		// Entity
		ENTITY_FLAGS,
		ENTITY_AIR,
		ENTITY_CUSTOM_NAME,
		ENTITY_CUSTOM_NAME_VISIBLE,
		ENTITY_SILENT,
		ENTITY_NO_GRAVITY,
		_ENTITY_NEXT,  // Used by descendants

		// Potion
		POTION_THROWN = _ENTITY_NEXT,

		// FallingBlock
		FALLING_BLOCK_POSITION = _ENTITY_NEXT,

		// AreaEffectCloud
		AREA_EFFECT_CLOUD_RADIUS = _ENTITY_NEXT,
		AREA_EFFECT_CLOUD_COLOR,
		AREA_EFFECT_CLOUD_SINGLE_POINT_EFFECT,
		AREA_EFFECT_CLOUD_PARTICLE_ID,
		AREA_EFFECT_CLOUD_PARTICLE_PARAMETER1,
		AREA_EFFECT_CLOUD_PARTICLE_PARAMETER2,

		// Arrow
		ARROW_CRITICAL = _ENTITY_NEXT,
		_ARROW_NEXT,

		// TippedArrow
		TIPPED_ARROW_COLOR = _ARROW_NEXT,

		// Boat
		BOAT_LAST_HIT_TIME = _ENTITY_NEXT,
		BOAT_FORWARD_DIRECTION,
		BOAT_DAMAGE_TAKEN,
		BOAT_TYPE,
		BOAT_RIGHT_PADDLE_TURNING,
		BOAT_LEFT_PADDLE_TURNING,

		// EnderCrystal
		ENDER_CRYSTAL_BEAM_TARGET = _ENTITY_NEXT,
		ENDER_CRYSTAL_SHOW_BOTTOM,

		// Fireball
		_FIREBALL_NEXT = _ENTITY_NEXT,

		// WitherSkull
		WITHER_SKULL_INVULNERABLE = _FIREBALL_NEXT,

		// Fireworks
		FIREWORK_INFO = _ENTITY_NEXT,
		FIREWORK_BOOSTED_ENTITY_ID,  // 1.11.1 only

		// Hanging
		_HANGING_NEXT = _ENTITY_NEXT,

		// ItemFrame
		ITEM_FRAME_ITEM = _HANGING_NEXT,
		ITEM_FRAME_ROTATION,

		// Item
		ITEM_ITEM = _ENTITY_NEXT,

		// Living
		LIVING_ACTIVE_HAND = _ENTITY_NEXT,
		LIVING_HEALTH,
		LIVING_POTION_EFFECT_COLOR,
		LIVING_POTION_EFFECT_AMBIENT,
		LIVING_NUMBER_OF_ARROWS,
		_LIVING_NEXT,

		// Player
		PLAYER_ADDITIONAL_HEARTHS = _LIVING_NEXT,
		PLAYER_SCORE,
		PLAYER_DISPLAYED_SKIN_PARTS,
		PLAYER_MAIN_HAND,

		// ArmorStand
		ARMOR_STAND_STATUS = _LIVING_NEXT,
		ARMOR_STAND_HEAD_ROTATION,
		ARMOR_STAND_BODY_ROTATION,
		ARMOR_STAND_LEFT_ARM_ROTATION,
		ARMOR_STAND_RIGHT_ARM_ROTATION,
		ARMOR_STAND_LEFT_LEG_ROTATION,
		ARMOR_STAND_RIGHT_LEG_ROTATION,

		// Insentient
		INSENTIENT_STATUS = _LIVING_NEXT,
		_INSENTIENT_NEXT,

		// Ambient
		_AMBIENT_NEXT = _INSENTIENT_NEXT,

		// Bat
		BAT_HANGING = _AMBIENT_NEXT,

		// Creature
		_CREATURE_NEXT = _INSENTIENT_NEXT,

		// Ageable
		AGEABLE_BABY = _CREATURE_NEXT,
		_AGEABLE_NEXT,

		// PolarBear
		POLAR_BEAR_STANDING = _AGEABLE_NEXT,

		// Animal
		_ANIMAL_NEXT = _AGEABLE_NEXT,

		// Abstract horse
		ABSTRACT_HORSE_STATUS = _ANIMAL_NEXT,
		ABSTRACT_HORSE_OWNER,
		_ABSTRACT_HORSE_NEXT,

		// Horse
		HORSE_VARIANT = _ABSTRACT_HORSE_NEXT,
		HORSE_ARMOR,

		// Chested horse
		CHESTED_HORSE_CHESTED = _ABSTRACT_HORSE_NEXT,
		_CHESTED_HORSE_NEXT,

		// Llama
		LLAMA_STRENGTH = _CHESTED_HORSE_NEXT,
		LLAMA_CARPET_COLOR,
		LLAMA_VARIANT,

		// Pig
		PIG_HAS_SADDLE = _ANIMAL_NEXT,
		PIG_TOTAL_CARROT_ON_A_STICK_BOOST,  // 1.11.1 only

		// Rabbit
		RABBIT_TYPE = _ANIMAL_NEXT,

		// Sheep
		SHEEP_STATUS = _ANIMAL_NEXT,

		// TameableAnimal
		TAMEABLE_ANIMAL_STATUS = _ANIMAL_NEXT,
		TAMEABLE_ANIMAL_OWNER,
		_TAMEABLE_NEXT,

		// Ocelot
		OCELOT_TYPE = _TAMEABLE_NEXT,

		// Wolf
		WOLF_DAMAGE_TAKEN = _TAMEABLE_NEXT,
		WOLF_BEGGING,
		WOLF_COLLAR_COLOR,

		// Villager
		VILLAGER_PROFESSION = _AGEABLE_NEXT,

		// Golem
		_GOLEM_NEXT = _CREATURE_NEXT,

		// IronGolem
		IRON_GOLEM_PLAYER_CREATED = _GOLEM_NEXT,

		// Shulker
		SHULKER_FACING_DIRECTION = _GOLEM_NEXT,
		SHULKER_ATTACHMENT_FALLING_BLOCK_POSITION,
		SHULKER_SHIELD_HEIGHT,

		// Monster
		_MONSTER_NEXT = _CREATURE_NEXT,

		// Blaze
		BLAZE_ON_FIRE = _MONSTER_NEXT,

		// Creeper
		CREEPER_STATE = _MONSTER_NEXT,
		CREEPER_POWERED,
		CREEPER_IGNITED,

		// Guardian
		GUARDIAN_STATUS = _MONSTER_NEXT,
		GUARDIAN_TARGET,

		// Abstract Skeleton
		ABSTRACT_SKELETON_ARMS_SWINGING = _MONSTER_NEXT,

		// Spider
		SPIDER_CLIMBING = _MONSTER_NEXT,

		// Witch
		WITCH_AGGRESIVE = _MONSTER_NEXT,

		// Wither
		WITHER_FIRST_HEAD_TARGET = _MONSTER_NEXT,
		WITHER_SECOND_HEAD_TARGET,
		WITHER_THIRD_HEAD_TARGET,
		WITHER_INVULNERABLE_TIMER,

		// Zombie
		ZOMBIE_IS_BABY = _MONSTER_NEXT,
		ZOMBIE_UNUSED,  // Was type
		ZOMBIE_HANDS_RISED_UP,
		_ZOMBIE_NEXT,

		// Zombie villager
		ZOMBIE_VILLAGER_CONVERTING = _ZOMBIE_NEXT,
		ZOMBIE_VILLAGER_PROFESSION,

		// Enderman
		ENDERMAN_CARRIED_BLOCK = _MONSTER_NEXT,
		ENDERMAN_SCREAMING,

		// Evocation illager
		EVOKER_SPELL = _MONSTER_NEXT,

		// Vex
		VEX_FLAGS = _MONSTER_NEXT,

		// Vindication illager
		VINDICATOR_FLAGS = _MONSTER_NEXT,

		// EnderDragon
		ENDER_DRAGON_DRAGON_PHASE = _INSENTIENT_NEXT,

		// Flying
		_FLYING_NEXT = _INSENTIENT_NEXT,

		// Ghast
		GHAST_ATTACKING = _FLYING_NEXT,

		// Slime
		SLIME_SIZE = _INSENTIENT_NEXT,

		// Minecart
		MINECART_SHAKING_POWER = _ENTITY_NEXT,
		MINECART_SHAKING_DIRECTION,
		MINECART_SHAKING_MULTIPLIER,
		MINECART_BLOCK_ID_META,
		MINECART_BLOCK_Y,
		MINECART_SHOW_BLOCK,
		_MINECART_NEXT,

		// MinecartCommandBlock
		MINECART_COMMAND_BLOCK_COMMAND = _MINECART_NEXT,
		MINECART_COMMAND_BLOCK_LAST_OUTPUT,

		// MinecartFurnace
		MINECART_FURNACE_POWERED = _MINECART_NEXT,

		// TNTPrimed
		TNT_PRIMED_FUSE_TIME = _ENTITY_NEXT,
	};
}

#ifdef __clang__
	#pragma clang diagnostic pop  // Restore ignored clang errors
#endif





#define HANDLE_READ(ByteBuf, Proc, Type, Var) \
	Type Var; \
	do { \
		if (!ByteBuf.Proc(Var))\
		{\
			return;\
		} \
	} while (false)





////////////////////////////////////////////////////////////////////////////////
// cProtocol_1_11_0:

void cProtocol_1_11_0::SendCollectEntity(const cEntity & a_Collected, const cEntity & a_Collector, unsigned a_Count)
{
	ASSERT(m_State == 3);  // In game mode?

	cPacketizer Pkt(*this, pktCollectEntity);
	Pkt.WriteVarInt32(a_Collected.GetUniqueID());
	Pkt.WriteVarInt32(a_Collector.GetUniqueID());
	Pkt.WriteVarInt32(static_cast<UInt32>(a_Count));
}





void cProtocol_1_11_0::SendEntityAnimation(const cEntity & a_Entity, const EntityAnimation a_Animation)
{
	switch (a_Animation)
	{
		case EntityAnimation::EggCracks:
		case EntityAnimation::SnowballPoofs:
		{
			// Vanilla stopped doing clientside prediction for thrown projectile particle effects (for some reason).
			// But they're still doing motion prediction, and latency exists, hence re-send the server position to avoid particle effects happening inside a block:
			SendEntityPosition(a_Entity);
			break;
		}
		case EntityAnimation::PawnChestEquipmentBreaks:
		case EntityAnimation::PawnFeetEquipmentBreaks:
		case EntityAnimation::PawnHeadEquipmentBreaks:
		case EntityAnimation::PawnLegsEquipmentBreaks:
		case EntityAnimation::PawnMainHandEquipmentBreaks:
		case EntityAnimation::PawnOffHandEquipmentBreaks:
		{
			const auto Position = a_Entity.GetPosition();

			// 1.11 dropped the automatic particle effect + sound on item break. Emulate at least some of it:
			SendSoundEffect("entity.item.break", Position.x, Position.y, Position.z, 1, 0.75f + ((a_Entity.GetUniqueID() * 23) % 32) / 64.f);
			break;
		}
		default: break;
	}

	Super::SendEntityAnimation(a_Entity, a_Animation);
}





void cProtocol_1_11_0::SendHideTitle(void)
{
	ASSERT(m_State == 3);  // In game mode?

	cPacketizer Pkt(*this, pktTitle);
	Pkt.WriteVarInt32(4);  // Hide title
}





void cProtocol_1_11_0::SendResetTitle(void)
{
	ASSERT(m_State == 3);  // In game mode?

	cPacketizer Pkt(*this, pktTitle);
	Pkt.WriteVarInt32(5);  // Reset title
}





void cProtocol_1_11_0::SendSpawnMob(const cMonster & a_Mob)
{
	ASSERT(m_State == 3);  // In game mode?

	const auto MobType = GetProtocolMobType(a_Mob.GetMobType());

	// If the type is not valid in this protocol bail out:
	if (MobType == 0)
	{
		return;
	}

	cPacketizer Pkt(*this, pktSpawnMob);
	Pkt.WriteVarInt32(a_Mob.GetUniqueID());
	// TODO: Bad way to write a UUID, and it's not a true UUID, but this is functional for now.
	Pkt.WriteBEUInt64(0);
	Pkt.WriteBEUInt64(a_Mob.GetUniqueID());
	Pkt.WriteVarInt32(MobType);
	Vector3d LastSentPos = a_Mob.GetLastSentPosition();
	Pkt.WriteBEDouble(LastSentPos.x);
	Pkt.WriteBEDouble(LastSentPos.y);
	Pkt.WriteBEDouble(LastSentPos.z);
	Pkt.WriteByteAngle(a_Mob.GetHeadYaw());  // Doesn't seem to be used
	Pkt.WriteByteAngle(a_Mob.GetPitch());
	Pkt.WriteByteAngle(a_Mob.GetYaw());
	Pkt.WriteBEInt16(static_cast<Int16>(a_Mob.GetSpeedX() * 400));
	Pkt.WriteBEInt16(static_cast<Int16>(a_Mob.GetSpeedY() * 400));
	Pkt.WriteBEInt16(static_cast<Int16>(a_Mob.GetSpeedZ() * 400));
	WriteEntityMetadata(Pkt, a_Mob);
	Pkt.WriteBEUInt8(0xff);  // Metadata terminator
}





void cProtocol_1_11_0::SendTitleTimes(int a_FadeInTicks, int a_DisplayTicks, int a_FadeOutTicks)
{
	ASSERT(m_State == 3);  // In game mode?

	cPacketizer Pkt(*this, pktTitle);
	Pkt.WriteVarInt32(3);  // Set title display times
	Pkt.WriteBEInt32(a_FadeInTicks);
	Pkt.WriteBEInt32(a_DisplayTicks);
	Pkt.WriteBEInt32(a_FadeOutTicks);
}





void cProtocol_1_11_0::SendUpdateBlockEntity(cBlockEntity & a_BlockEntity)
{
	ASSERT(m_State == 3);  // In game mode?

	Byte Action;
	switch (a_BlockEntity.GetBlockType())
	{
		case E_BLOCK_ENCHANTMENT_TABLE: Action = 0;  break;  // The ones with a action of 0 is just a workaround to send the block entities to a client.
		case E_BLOCK_END_PORTAL:        Action = 0;  break;  // Todo: 18.09.2020 - remove this when block entities are transmitted in the ChunkData packet - 12xx12

		case E_BLOCK_MOB_SPAWNER:       Action = 1;  break;  // Update mob spawner spinny mob thing
		case E_BLOCK_COMMAND_BLOCK:     Action = 2;  break;  // Update command block text
		case E_BLOCK_BEACON:            Action = 3;  break;  // Update beacon entity
		case E_BLOCK_HEAD:              Action = 4;  break;  // Update mobhead entity
		case E_BLOCK_FLOWER_POT:        Action = 5;  break;  // Update flower pot
		case E_BLOCK_WALL_BANNER:
		case E_BLOCK_STANDING_BANNER:   Action = 6;  break;  // Update banner
		case E_BLOCK_BED:               Action = 11; break;  // Update bed color (new!)

		default: return;  // Block entities change between versions
	}

	cPacketizer Pkt(*this, pktUpdateBlockEntity);
	Pkt.WriteXYZPosition64(a_BlockEntity.GetPosX(), a_BlockEntity.GetPosY(), a_BlockEntity.GetPosZ());
	Pkt.WriteBEUInt8(Action);

	cFastNBTWriter Writer;
	WriteBlockEntity(Writer, a_BlockEntity);
	Writer.Finish();
	Pkt.WriteBuf(Writer.GetResult());
}





signed char cProtocol_1_11_0::GetProtocolEntityStatus(const EntityAnimation a_Animation) const
{
	switch (a_Animation)
	{
		case EntityAnimation::EggCracks: return 3;
		case EntityAnimation::EvokerFangsAttacks: return 4;
		case EntityAnimation::IronGolemStashesGift: return 34;
		case EntityAnimation::PawnTotemActivates: return 35;
		case EntityAnimation::SnowballPoofs: return 3;
		default: return Super::GetProtocolEntityStatus(a_Animation);
	}
}





UInt32 cProtocol_1_11_0::GetProtocolMobType(const eMonsterType a_MobType) const
{
	switch (a_MobType)
	{
		// Map invalid type to Giant for easy debugging (if this ever spawns, something has gone very wrong)
		case mtInvalidType:           return 53;
		case mtBat:                   return 65;
		case mtBlaze:                 return 61;
		case mtCaveSpider:            return 59;
		case mtChicken:               return 93;
		case mtCow:                   return 92;
		case mtCreeper:               return 50;
		case mtDonkey:                return 31;
		case mtEnderDragon:           return 63;
		case mtEnderman:              return 58;
		case mtEndermite:             return 67;
		case mtEvoker:                return 34;
		case mtGhast:                 return 56;
		case mtGiant:                 return 53;
		case mtGuardian:              return 68;
		case mtHorse:                 return 100;
		case mtHusk:                  return 23;
		case mtIronGolem:             return 99;
		case mtLlama:                 return 103;
		case mtMagmaCube:             return 62;
		case mtMooshroom:             return 96;
		case mtMule:                  return 32;
		case mtOcelot:                return 98;
		case mtPig:                   return 90;
		case mtPolarBear:             return 102;
		case mtRabbit:                return 101;
		case mtSheep:                 return 91;
		case mtShulker:               return 69;
		case mtSilverfish:            return 60;
		case mtSkeleton:              return 51;
		case mtSlime:                 return 55;
		case mtSnowGolem:             return 97;
		case mtSpider:                return 52;
		case mtStray:                 return 6;
		case mtTraderLlama:           return 103;
		case mtSquid:                 return 94;
		case mtVex:                   return 35;
		case mtVillager:              return 120;
		case mtVindicator:            return 36;
		case mtWitch:                 return 66;
		case mtWither:                return 64;
		case mtWitherSkeleton:        return 5;
		case mtWolf:                  return 95;
		case mtZombie:                return 54;
		case mtZombiePigman:          return 57;
		case mtZombieVillager:        return 27;

		default:                      return 0;
	}
}





cProtocol::Version cProtocol_1_11_0::GetProtocolVersion() const
{
	return Version::v1_11_0;
}





void cProtocol_1_11_0::HandlePacketBlockPlace(cByteBuffer & a_ByteBuffer)
{
	int BlockX, BlockY, BlockZ;
	if (!a_ByteBuffer.ReadXYZPosition64(BlockX, BlockY, BlockZ))
	{
		return;
	}

	HANDLE_READ(a_ByteBuffer, ReadVarInt, Int32, Face);
	HANDLE_READ(a_ByteBuffer, ReadVarInt, Int32, Hand);
	HANDLE_READ(a_ByteBuffer, ReadBEFloat, float, CursorX);
	HANDLE_READ(a_ByteBuffer, ReadBEFloat, float, CursorY);
	HANDLE_READ(a_ByteBuffer, ReadBEFloat, float, CursorZ);

	m_Client->HandleRightClick(BlockX, BlockY, BlockZ, FaceIntToBlockFace(Face), FloorC(CursorX * 16), FloorC(CursorY * 16), FloorC(CursorZ * 16), Hand == 0);
}





void cProtocol_1_11_0::WriteBlockEntity(cFastNBTWriter & a_Writer, const cBlockEntity & a_BlockEntity) const
{
	a_Writer.AddInt("x", a_BlockEntity.GetPosX());
	a_Writer.AddInt("y", a_BlockEntity.GetPosY());
	a_Writer.AddInt("z", a_BlockEntity.GetPosZ());

	switch (a_BlockEntity.GetBlockType())
	{
		case E_BLOCK_BED:
		{
			auto & BedEntity = static_cast<const cBedEntity &>(a_BlockEntity);
			a_Writer.AddInt("color", BedEntity.GetColor());  // New: multicoloured beds
			break;
		}

		case E_BLOCK_MOB_SPAWNER:
		{
			auto & MobSpawnerEntity = static_cast<const cMobSpawnerEntity &>(a_BlockEntity);
			a_Writer.BeginCompound("SpawnData");
				a_Writer.AddString("id", cMonster::MobTypeToVanillaNBT(MobSpawnerEntity.GetEntity()));  // New: uses namespaced ID
			a_Writer.EndCompound();
			a_Writer.AddShort("Delay", MobSpawnerEntity.GetSpawnDelay());
			break;
		}

		default: Super::WriteBlockEntity(a_Writer, a_BlockEntity);
	}
}





void cProtocol_1_11_0::WriteEntityMetadata(cPacketizer & a_Pkt, const cEntity & a_Entity) const
{
	using namespace Metadata_1_11;

	// Common metadata:
	Int8 Flags = 0;
	if (a_Entity.IsOnFire())
	{
		Flags |= 0x01;
	}
	if (a_Entity.IsCrouched())
	{
		Flags |= 0x02;
	}
	if (a_Entity.IsSprinting())
	{
		Flags |= 0x08;
	}
	if (a_Entity.IsRclking())
	{
		Flags |= 0x10;
	}
	if (a_Entity.IsInvisible())
	{
		Flags |= 0x20;
	}
	a_Pkt.WriteBEUInt8(ENTITY_FLAGS);  // Index
	a_Pkt.WriteBEUInt8(METADATA_TYPE_BYTE);  // Type
	a_Pkt.WriteBEInt8(Flags);

	switch (a_Entity.GetEntityType())
	{
		case cEntity::etPlayer:
		{
			auto & Player = static_cast<const cPlayer &>(a_Entity);

			// TODO Set player custom name to their name.
			// Then it's possible to move the custom name of mobs to the entities
			// and to remove the "special" player custom name.
			a_Pkt.WriteBEUInt8(ENTITY_CUSTOM_NAME);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_STRING);
			a_Pkt.WriteString(Player.GetName());

			a_Pkt.WriteBEUInt8(LIVING_HEALTH);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_FLOAT);
			a_Pkt.WriteBEFloat(static_cast<float>(Player.GetHealth()));

			a_Pkt.WriteBEUInt8(PLAYER_DISPLAYED_SKIN_PARTS);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_BYTE);
			a_Pkt.WriteBEUInt8(static_cast<UInt8>(Player.GetSkinParts()));

			a_Pkt.WriteBEUInt8(PLAYER_MAIN_HAND);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_BYTE);
			a_Pkt.WriteBEUInt8(Player.IsLeftHanded() ? 0 : 1);
			break;
		}
		case cEntity::etPickup:
		{
			a_Pkt.WriteBEUInt8(ITEM_ITEM);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_ITEM);
			WriteItem(a_Pkt, static_cast<const cPickup &>(a_Entity).GetItem());
			break;
		}
		case cEntity::etMinecart:
		{
			a_Pkt.WriteBEUInt8(MINECART_SHAKING_POWER);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_VARINT);

			// The following expression makes Minecarts shake more with less health or higher damage taken
			auto & Minecart = static_cast<const cMinecart &>(a_Entity);
			auto maxHealth = a_Entity.GetMaxHealth();
			auto curHealth = a_Entity.GetHealth();
			a_Pkt.WriteVarInt32(static_cast<UInt32>((maxHealth - curHealth) * Minecart.LastDamage() * 4));

			a_Pkt.WriteBEUInt8(MINECART_SHAKING_DIRECTION);  // (doesn't seem to effect anything)
			a_Pkt.WriteBEUInt8(METADATA_TYPE_VARINT);
			a_Pkt.WriteVarInt32(1);

			a_Pkt.WriteBEUInt8(MINECART_SHAKING_MULTIPLIER);  // or damage taken
			a_Pkt.WriteBEUInt8(METADATA_TYPE_FLOAT);
			a_Pkt.WriteBEFloat(static_cast<float>(Minecart.LastDamage() + 10));

			if (Minecart.GetPayload() == cMinecart::mpNone)
			{
				auto & RideableMinecart = static_cast<const cRideableMinecart &>(Minecart);
				const cItem & MinecartContent = RideableMinecart.GetContent();
				if (!MinecartContent.IsEmpty())
				{
					a_Pkt.WriteBEUInt8(MINECART_BLOCK_ID_META);
					a_Pkt.WriteBEUInt8(METADATA_TYPE_VARINT);
					int Content = MinecartContent.m_ItemType;
					Content |= MinecartContent.m_ItemDamage << 8;
					a_Pkt.WriteVarInt32(static_cast<UInt32>(Content));

					a_Pkt.WriteBEUInt8(MINECART_BLOCK_Y);
					a_Pkt.WriteBEUInt8(METADATA_TYPE_VARINT);
					a_Pkt.WriteVarInt32(static_cast<UInt32>(RideableMinecart.GetBlockHeight()));

					a_Pkt.WriteBEUInt8(MINECART_SHOW_BLOCK);
					a_Pkt.WriteBEUInt8(METADATA_TYPE_BOOL);
					a_Pkt.WriteBool(true);
				}
			}
			else if (Minecart.GetPayload() == cMinecart::mpFurnace)
			{
				a_Pkt.WriteBEUInt8(MINECART_FURNACE_POWERED);
				a_Pkt.WriteBEUInt8(METADATA_TYPE_BOOL);
				a_Pkt.WriteBool(static_cast<const cMinecartWithFurnace &>(Minecart).IsFueled());
			}
			break;
		}  // case etMinecart

		case cEntity::etProjectile:
		{
			auto & Projectile = static_cast<const cProjectileEntity &>(a_Entity);
			switch (Projectile.GetProjectileKind())
			{
				case cProjectileEntity::pkArrow:
				{
					a_Pkt.WriteBEUInt8(ARROW_CRITICAL);
					a_Pkt.WriteBEUInt8(METADATA_TYPE_BYTE);
					a_Pkt.WriteBEInt8(static_cast<const cArrowEntity &>(Projectile).IsCritical() ? 1 : 0);
					break;
				}
				case cProjectileEntity::pkFirework:
				{
					a_Pkt.WriteBEUInt8(FIREWORK_INFO);  // Firework item used for this firework
					a_Pkt.WriteBEUInt8(METADATA_TYPE_ITEM);
					WriteItem(a_Pkt, static_cast<const cFireworkEntity &>(Projectile).GetItem());

					// FIREWORK_BOOSTED_ENTITY_ID, in 1.11.1 only
					break;
				}
				case cProjectileEntity::pkSplashPotion:
				{
					a_Pkt.WriteBEUInt8(POTION_THROWN);  // Potion item which was thrown
					a_Pkt.WriteBEUInt8(METADATA_TYPE_ITEM);
					WriteItem(a_Pkt, static_cast<const cSplashPotionEntity &>(Projectile).GetItem());
				}
				default:
				{
					break;
				}
			}
			break;
		}  // case etProjectile

		case cEntity::etMonster:
		{
			WriteMobMetadata(a_Pkt, static_cast<const cMonster &>(a_Entity));
			break;
		}

		case cEntity::etBoat:
		{
			auto & Boat = static_cast<const cBoat &>(a_Entity);

			a_Pkt.WriteBEInt8(BOAT_LAST_HIT_TIME);
			a_Pkt.WriteBEInt8(METADATA_TYPE_VARINT);
			a_Pkt.WriteVarInt32(static_cast<UInt32>(Boat.GetLastDamage()));

			a_Pkt.WriteBEInt8(BOAT_FORWARD_DIRECTION);
			a_Pkt.WriteBEInt8(METADATA_TYPE_VARINT);
			a_Pkt.WriteVarInt32(static_cast<UInt32>(Boat.GetForwardDirection()));

			a_Pkt.WriteBEInt8(BOAT_DAMAGE_TAKEN);
			a_Pkt.WriteBEInt8(METADATA_TYPE_FLOAT);
			a_Pkt.WriteBEFloat(Boat.GetDamageTaken());

			a_Pkt.WriteBEInt8(BOAT_TYPE);
			a_Pkt.WriteBEInt8(METADATA_TYPE_VARINT);
			a_Pkt.WriteVarInt32(static_cast<UInt32>(Boat.GetMaterial()));

			a_Pkt.WriteBEInt8(BOAT_RIGHT_PADDLE_TURNING);
			a_Pkt.WriteBEInt8(METADATA_TYPE_BOOL);
			a_Pkt.WriteBool(Boat.IsRightPaddleUsed());

			a_Pkt.WriteBEInt8(BOAT_LEFT_PADDLE_TURNING);
			a_Pkt.WriteBEInt8(METADATA_TYPE_BOOL);
			a_Pkt.WriteBool(Boat.IsLeftPaddleUsed());

			break;
		}  // case etBoat

		case cEntity::etItemFrame:
		{
			auto & Frame = static_cast<const cItemFrame &>(a_Entity);
			a_Pkt.WriteBEUInt8(ITEM_FRAME_ITEM);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_ITEM);
			WriteItem(a_Pkt, Frame.GetItem());
			a_Pkt.WriteBEUInt8(ITEM_FRAME_ROTATION);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_VARINT);
			a_Pkt.WriteVarInt32(Frame.GetItemRotation());
			break;
		}  // case etItemFrame

		case cEntity::etEnderCrystal:
		{
			const auto & EnderCrystal = static_cast<const cEnderCrystal &>(a_Entity);
			if (EnderCrystal.DisplaysBeam())
			{
				a_Pkt.WriteBEUInt8(ENDER_CRYSTAL_BEAM_TARGET);
				a_Pkt.WriteBEUInt8(METADATA_TYPE_OPTIONAL_POSITION);
				a_Pkt.WriteBool(true);  // Dont do a second check if it should display the beam
				a_Pkt.WriteXYZPosition64(EnderCrystal.GetBeamTarget());
			}
			a_Pkt.WriteBEUInt8(ENDER_CRYSTAL_SHOW_BOTTOM);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_BOOL);
			a_Pkt.WriteBool(EnderCrystal.ShowsBottom());
			break;
		}  // case etEnderCrystal

		default:
		{
			break;
		}
	}
}





void cProtocol_1_11_0::WriteMobMetadata(cPacketizer & a_Pkt, const cMonster & a_Mob) const
{
	using namespace Metadata_1_11;

	// Living entity Metadata
	if (a_Mob.HasCustomName())
	{
		// TODO: As of 1.9 _all_ entities can have custom names; should this be moved up?
		a_Pkt.WriteBEUInt8(ENTITY_CUSTOM_NAME);
		a_Pkt.WriteBEUInt8(METADATA_TYPE_STRING);
		a_Pkt.WriteString(a_Mob.GetCustomName());

		a_Pkt.WriteBEUInt8(ENTITY_CUSTOM_NAME_VISIBLE);  // Custom name always visible
		a_Pkt.WriteBEUInt8(METADATA_TYPE_BOOL);
		a_Pkt.WriteBool(a_Mob.IsCustomNameAlwaysVisible());
	}

	a_Pkt.WriteBEUInt8(LIVING_HEALTH);
	a_Pkt.WriteBEUInt8(METADATA_TYPE_FLOAT);
	a_Pkt.WriteBEFloat(static_cast<float>(a_Mob.GetHealth()));

	switch (a_Mob.GetMobType())
	{
		case mtBat:
		{
			auto & Bat = static_cast<const cBat &>(a_Mob);
			a_Pkt.WriteBEUInt8(BAT_HANGING);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_BYTE);
			a_Pkt.WriteBEInt8(Bat.IsHanging() ? 1 : 0);
			break;
		}  // case mtBat

		case mtChicken:
		{
			auto & Chicken = static_cast<const cChicken &>(a_Mob);

			a_Pkt.WriteBEUInt8(AGEABLE_BABY);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_BOOL);
			a_Pkt.WriteBool(Chicken.IsBaby());
			break;
		}  // case mtChicken

		case mtCow:
		{
			auto & Cow = static_cast<const cCow &>(a_Mob);

			a_Pkt.WriteBEUInt8(AGEABLE_BABY);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_BOOL);
			a_Pkt.WriteBool(Cow.IsBaby());
			break;
		}  // case mtCow

		case mtCreeper:
		{
			auto & Creeper = static_cast<const cCreeper &>(a_Mob);
			a_Pkt.WriteBEUInt8(CREEPER_STATE);  // (idle or "blowing")
			a_Pkt.WriteBEUInt8(METADATA_TYPE_VARINT);
			a_Pkt.WriteVarInt32(Creeper.IsBlowing() ? 1 : static_cast<UInt32>(-1));

			a_Pkt.WriteBEUInt8(CREEPER_POWERED);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_BOOL);
			a_Pkt.WriteBool(Creeper.IsCharged());

			a_Pkt.WriteBEUInt8(CREEPER_IGNITED);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_BOOL);
			a_Pkt.WriteBool(Creeper.IsBurnedWithFlintAndSteel());
			break;
		}  // case mtCreeper

		case mtEnderman:
		{
			auto & Enderman = static_cast<const cEnderman &>(a_Mob);
			a_Pkt.WriteBEUInt8(ENDERMAN_CARRIED_BLOCK);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_BLOCKID);
			UInt32 Carried = 0;
			Carried |= static_cast<UInt32>(Enderman.GetCarriedBlock() << 4);
			Carried |= Enderman.GetCarriedMeta();
			a_Pkt.WriteVarInt32(Carried);

			a_Pkt.WriteBEUInt8(ENDERMAN_SCREAMING);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_BOOL);
			a_Pkt.WriteBool(Enderman.IsScreaming());
			break;
		}  // case mtEnderman

		case mtGhast:
		{
			auto & Ghast = static_cast<const cGhast &>(a_Mob);
			a_Pkt.WriteBEUInt8(GHAST_ATTACKING);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_BOOL);
			a_Pkt.WriteBool(Ghast.IsCharging());
			break;
		}  // case mtGhast

		case mtHorse:
		{
			// XXX This behaves incorrectly with different variants; horses have different entity IDs now

			// Abstract horse
			auto & Horse = static_cast<const cHorse &>(a_Mob);
			Int8 Flags = 0;
			if (Horse.IsTame())
			{
				Flags |= 0x02;
			}
			if (Horse.IsSaddled())
			{
				Flags |= 0x04;
			}
			if (Horse.IsChested())
			{
				Flags |= 0x08;
			}
			if (Horse.IsEating())
			{
				Flags |= 0x20;
			}
			if (Horse.IsRearing())
			{
				Flags |= 0x40;
			}
			if (Horse.IsMthOpen())
			{
				Flags |= 0x80;
			}
			a_Pkt.WriteBEUInt8(ABSTRACT_HORSE_STATUS);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_BYTE);
			a_Pkt.WriteBEInt8(Flags);

			// This doesn't exist any more; it'll cause horses to all be the normal type
			// a_Pkt.WriteBEUInt8(HORSE_TYPE);
			// a_Pkt.WriteBEUInt8(METADATA_TYPE_VARINT);
			// a_Pkt.WriteVarInt32(static_cast<UInt32>(Horse.GetHorseType()));

			// Regular horses
			a_Pkt.WriteBEUInt8(HORSE_VARIANT);  // Color / style
			a_Pkt.WriteBEUInt8(METADATA_TYPE_VARINT);
			int Appearance = 0;
			Appearance = Horse.GetHorseColor();
			Appearance |= Horse.GetHorseStyle() << 8;
			a_Pkt.WriteVarInt32(static_cast<UInt32>(Appearance));

			a_Pkt.WriteBEUInt8(HORSE_ARMOR);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_VARINT);
			a_Pkt.WriteVarInt32(static_cast<UInt32>(Horse.GetHorseArmour()));

			a_Pkt.WriteBEUInt8(AGEABLE_BABY);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_BOOL);
			a_Pkt.WriteBool(Horse.IsBaby());
			break;
		}  // case mtHorse

		case mtMagmaCube:
		{
			auto & MagmaCube = static_cast<const cMagmaCube &>(a_Mob);
			a_Pkt.WriteBEUInt8(SLIME_SIZE);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_VARINT);
			a_Pkt.WriteVarInt32(static_cast<UInt32>(MagmaCube.GetSize()));
			break;
		}  // case mtMagmaCube

		case mtOcelot:
		{
			// Todo: move unnecessary to cat
			auto & Ocelot = static_cast<const cOcelot &>(a_Mob);

			a_Pkt.WriteBEUInt8(AGEABLE_BABY);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_BOOL);
			a_Pkt.WriteBool(Ocelot.IsBaby());

			Int8 OcelotStatus = 0;
			if (Ocelot.IsSitting())
			{
				OcelotStatus |= 0x1;
			}
			if (Ocelot.IsTame())
			{
				OcelotStatus |= 0x4;
			}
			a_Pkt.WriteBEUInt8(TAMEABLE_ANIMAL_STATUS);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_BYTE);
			a_Pkt.WriteBEInt8(OcelotStatus);

			a_Pkt.WriteBEUInt8(OCELOT_TYPE);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_VARINT);
			a_Pkt.WriteVarInt32(static_cast<UInt32>(Ocelot.GetOcelotType()));

			break;
		}  // case mtOcelot

		case mtPig:
		{
			auto & Pig = static_cast<const cPig &>(a_Mob);

			a_Pkt.WriteBEUInt8(AGEABLE_BABY);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_BOOL);
			a_Pkt.WriteBool(Pig.IsBaby());

			a_Pkt.WriteBEUInt8(PIG_HAS_SADDLE);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_BOOL);
			a_Pkt.WriteBool(Pig.IsSaddled());

			// PIG_TOTAL_CARROT_ON_A_STICK_BOOST in 1.11.1 only
			break;
		}  // case mtPig

		case mtRabbit:
		{
			auto & Rabbit = static_cast<const cRabbit &>(a_Mob);
			a_Pkt.WriteBEUInt8(AGEABLE_BABY);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_BOOL);
			a_Pkt.WriteBool(Rabbit.IsBaby());

			a_Pkt.WriteBEUInt8(RABBIT_TYPE);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_VARINT);
			a_Pkt.WriteVarInt32(static_cast<UInt32>(Rabbit.GetRabbitType()));
			break;
		}  // case mtRabbit

		case mtSheep:
		{
			auto & Sheep = static_cast<const cSheep &>(a_Mob);

			a_Pkt.WriteBEUInt8(AGEABLE_BABY);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_BOOL);
			a_Pkt.WriteBool(Sheep.IsBaby());

			a_Pkt.WriteBEUInt8(SHEEP_STATUS);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_BYTE);
			Int8 SheepMetadata = 0;
			SheepMetadata = static_cast<Int8>(Sheep.GetFurColor());
			if (Sheep.IsSheared())
			{
				SheepMetadata |= 0x10;
			}
			a_Pkt.WriteBEInt8(SheepMetadata);
			break;
		}  // case mtSheep

		case mtSlime:
		{
			auto & Slime = static_cast<const cSlime &>(a_Mob);
			a_Pkt.WriteBEUInt8(SLIME_SIZE);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_VARINT);
			a_Pkt.WriteVarInt32(static_cast<UInt32>(Slime.GetSize()));
			break;
		}  // case mtSlime

		case mtVillager:
		{
			auto & Villager = static_cast<const cVillager &>(a_Mob);
			a_Pkt.WriteBEUInt8(AGEABLE_BABY);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_BOOL);
			a_Pkt.WriteBool(Villager.IsBaby());

			a_Pkt.WriteBEUInt8(VILLAGER_PROFESSION);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_VARINT);
			a_Pkt.WriteVarInt32(static_cast<UInt32>(Villager.GetVilType()));
			break;
		}  // case mtVillager

		case mtWitch:
		{
			auto & Witch = static_cast<const cWitch &>(a_Mob);
			a_Pkt.WriteBEUInt8(WITCH_AGGRESIVE);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_BOOL);
			a_Pkt.WriteBool(Witch.IsAngry());
			break;
		}  // case mtWitch

		case mtWither:
		{
			auto & Wither = static_cast<const cWither &>(a_Mob);
			a_Pkt.WriteBEUInt8(WITHER_INVULNERABLE_TIMER);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_VARINT);
			a_Pkt.WriteVarInt32(Wither.GetWitherInvulnerableTicks());

			// TODO: Use boss bar packet for health
			break;
		}  // case mtWither

		case mtWolf:
		{
			auto & Wolf = static_cast<const cWolf &>(a_Mob);
			a_Pkt.WriteBEUInt8(AGEABLE_BABY);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_BOOL);
			a_Pkt.WriteBool(Wolf.IsBaby());

			Int8 WolfStatus = 0;
			if (Wolf.IsSitting())
			{
				WolfStatus |= 0x1;
			}
			if (Wolf.IsAngry())
			{
				WolfStatus |= 0x2;
			}
			if (Wolf.IsTame())
			{
				WolfStatus |= 0x4;
			}
			a_Pkt.WriteBEUInt8(TAMEABLE_ANIMAL_STATUS);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_BYTE);
			a_Pkt.WriteBEInt8(WolfStatus);

			a_Pkt.WriteBEUInt8(WOLF_DAMAGE_TAKEN);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_FLOAT);
			a_Pkt.WriteBEFloat(static_cast<float>(a_Mob.GetHealth()));  // TODO Not use the current health

			a_Pkt.WriteBEUInt8(WOLF_BEGGING);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_BOOL);
			a_Pkt.WriteBool(Wolf.IsBegging());

			a_Pkt.WriteBEUInt8(WOLF_COLLAR_COLOR);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_VARINT);
			a_Pkt.WriteVarInt32(static_cast<UInt32>(Wolf.GetCollarColor()));
			break;
		}  // case mtWolf

		case mtZombie:
		{
			// XXX Zombies were also split into new subclasses; this doesn't handle that.
			auto & Zombie = static_cast<const cZombie &>(a_Mob);
			a_Pkt.WriteBEUInt8(ZOMBIE_IS_BABY);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_BOOL);
			a_Pkt.WriteBool(Zombie.IsBaby());
			break;
		}  // case mtZombie

		case mtZombiePigman:
		{
			auto & ZombiePigman = static_cast<const cZombiePigman &>(a_Mob);
			a_Pkt.WriteBEUInt8(AGEABLE_BABY);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_BOOL);
			a_Pkt.WriteBool(ZombiePigman.IsBaby());
			break;
		}  // case mtZombiePigman

		case mtZombieVillager:
		{
			auto & ZombieVillager = reinterpret_cast<const cZombieVillager &>(a_Mob);
			a_Pkt.WriteBEUInt8(ZOMBIE_IS_BABY);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_BOOL);
			a_Pkt.WriteBool(ZombieVillager.IsBaby());

			a_Pkt.WriteBEUInt8(ZOMBIE_VILLAGER_CONVERTING);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_VARINT);
			a_Pkt.WriteVarInt32(static_cast<UInt32>(ZombieVillager.ConversionTime()));

			a_Pkt.WriteBEUInt8(ZOMBIE_VILLAGER_PROFESSION);
			a_Pkt.WriteBEUInt8(METADATA_TYPE_VARINT);
			a_Pkt.WriteVarInt32(static_cast<UInt32>(ZombieVillager.GetProfession()));
			break;
		}  // case mtZombieVillager

		case mtBlaze:
		case mtElderGuardian:
		case mtGuardian:
		case mtSnowGolem:
		{
			// TODO: Mobs with extra fields that aren't implemented
			break;
		}

		case mtCat:

		case mtDonkey:
		case mtMule:

		case mtEndermite:

		case mtEvoker:

		case mtLlama:

		case mtPolarBear:

		case mtShulker:

		case mtSkeletonHorse:
		case mtZombieHorse:

		case mtVex:

		case mtVindicator:
		{
			// Todo: Mobs not added yet. Grouped ones have the same metadata
			ASSERT(!"cProtocol_1_11::WriteMobMetadata: received unimplemented type");
			break;
		}

		case mtCaveSpider:
		case mtEnderDragon:
		case mtGiant:
		case mtIronGolem:
		case mtMooshroom:
		case mtSilverfish:
		case mtSkeleton:
		case mtStray:
		case mtSpider:
		case mtSquid:
		case mtWitherSkeleton:
		{
			// Mobs without additional metadata
			break;
		}

		default: UNREACHABLE("cProtocol_1_11::WriteMobMetadata: received mob of invalid type");
	}  // switch (a_Mob.GetType())
}





////////////////////////////////////////////////////////////////////////////////
// cProtocol_1_11_1:

cProtocol::Version cProtocol_1_11_1::GetProtocolVersion() const
{
	return Version::v1_11_1;
}
