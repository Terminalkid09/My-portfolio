import aiohttp
import discord
from discord.ext import commands

class SocialScan(commands.Cog):
    def __init__(self, bot):
        self.bot = bot

    @commands.command()
    async def social(self, ctx, username: str):
        """Controlla se un username esiste su vari social"""

        await ctx.send(f"🔍 Scansione social per: `{username}`")

        platforms = {
            "Instagram": f"https://www.instagram.com/{username}/",
            "Twitter": f"https://x.com/{username}",
            "TikTok": f"https://www.tiktok.com/@{username}",
            "GitHub": f"https://github.com/{username}",
            "Reddit": f"https://www.reddit.com/user/{username}",
            "Steam": f"https://steamcommunity.com/id/{username}",
            "Pinterest": f"https://www.pinterest.com/{username}/",
        }

        results = {}

        async with aiohttp.ClientSession() as session:
            for name, url in platforms.items():
                try:
                    async with session.get(url, allow_redirects=True) as response:
                        if response.status == 200:
                            results[name] = url
                        else:
                            results[name] = None
                except:
                    results[name] = None

        embed = discord.Embed(
            title=f"🌐 Social Scan — {username}",
            color=discord.Color.blue()
        )

        for platform, link in results.items():
            if link:
                embed.add_field(
                    name=f"🟢 {platform}",
                    value=f"[Profilo trovato]({link})",
                    inline=False
                )
            else:
                embed.add_field(
                    name=f"🔴 {platform}",
                    value="Non trovato",
                    inline=False
                )

        embed.set_footer(text="OSINT Social Scanner")
        await ctx.send(embed=embed)

async def setup(bot):
    await bot.add_cog(SocialScan(bot))