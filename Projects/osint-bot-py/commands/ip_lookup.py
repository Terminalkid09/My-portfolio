import aiohttp
import discord
from discord.ext import commands

class IpLookup(commands.Cog):
    def __init__(self, bot):
        self.bot = bot

    @commands.command()
    async def ip(self, ctx, ip_address: str):
        """Lookup informazioni su un indirizzo IP"""

        url = f"http://ip-api.com/json/{ip_address}?fields=66846719"

        async with aiohttp.ClientSession() as session:
            async with session.get(url) as response:
                data = await response.json()

        if data["status"] != "success":
            await ctx.send("❌ IP non valido o impossibile da analizzare.")
            return

        embed = discord.Embed(
            title=f"🔍 IP Lookup: {ip_address}",
            color=discord.Color.blue()
        )

        embed.add_field(name="🌍 Paese", value=data.get("country", "N/A"), inline=True)
        embed.add_field(name="🏙️ Regione", value=data.get("regionName", "N/A"), inline=True)
        embed.add_field(name="📍 Città", value=data.get("city", "N/A"), inline=True)
        embed.add_field(name="🌐 ISP", value=data.get("isp", "N/A"), inline=True)
        embed.add_field(name="🏢 ASN", value=data.get("as", "N/A"), inline=True)
        embed.add_field(name="🛰️ Lat/Lon", value=f"{data.get('lat')}, {data.get('lon')}", inline=True)

        embed.set_footer(text="Powered by ip-api.com")

        await ctx.send(embed=embed)

async def setup(bot):
    await bot.add_cog(IpLookup(bot))