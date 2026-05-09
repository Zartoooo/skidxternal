#include "math.hpp"
#include "../classes/GameContext.hpp"
#include "../sdk/sdk.hpp"

Vec2 CMaths::WorldToScreen(const Vec3& world, bool& onScreen)
{
	Quaternion quaternion;
	Vec2 dimensions = Game.VisengineCache.Dimensions;
	Matrix4x4 view_matrix = Game.VisengineCache.ViewMatrix;

	quaternion.x = (world.x * view_matrix[0]) + (world.y * view_matrix[1]) + (world.z * view_matrix[2]) + view_matrix[3];
	quaternion.y = (world.x * view_matrix[4]) + (world.y * view_matrix[5]) + (world.z * view_matrix[6]) + view_matrix[7];
	quaternion.z = (world.x * view_matrix[8]) + (world.y * view_matrix[9]) + (world.z * view_matrix[10]) + view_matrix[11];
	quaternion.w = (world.x * view_matrix[12]) + (world.y * view_matrix[13]) + (world.z * view_matrix[14]) + view_matrix[15];
	Vec2 screen;
	if (quaternion.w < 0.1f) {
		onScreen = false;
		return screen;
	}
	Vec3 normalize_device_coordinates;
	normalize_device_coordinates.x = quaternion.x / quaternion.w;
	normalize_device_coordinates.y = quaternion.y / quaternion.w;
	normalize_device_coordinates.z = quaternion.z / quaternion.w;
	screen.x = (dimensions.x / 2.0f * normalize_device_coordinates.x) + (dimensions.x / 2.0f);
	screen.y = -(dimensions.y / 2.0f * normalize_device_coordinates.y) + (dimensions.y / 2.0f);
	onScreen = true;
	return screen;
}
